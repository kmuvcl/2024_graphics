# TODO list
각 정점의 컬러를 Phong reflection 모델로 결정하는 Gouraud shading을 다음과 같이 구현한다.

## 쉐이더 코드에 Phong reflection 알고리즘 구현
* 각 정점의 컬러를 결정하기 위해 `vertex.glsl` 코드에 Phong reflection 알고리즘을 구현한다.
* Phong reflection 알고리즘이 구성될 수 있도록 추가적인 `uniform` 변수 혹은 `attribute` 변수를 선언한다.
* `vertex.glsl` 코드의 `vec3 directional_light()` 함수에서 각 정점의 컬러를 ambient, diffuse, specular 컬러의 합으로 결정하는 Phong reflection 알고리즘 코드를 구성한다. 

## C++ 코드에서 쉐이더 코드로 데이터 전달하기
* C++ 코드, 즉 `main.cpp`에서 `vertex.glsl` 코드에 데이터를 전달하기 위한 코드를 구성한다.
* 먼저 GPU의 `uniform` 혹은 `attribute` 값들이 저장되는 위치를 받아서 저장할 `GLint` 타입의 변수들을 선언한다.

```cpp
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_normal;     // attribute 변수 a_normal 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치

GLint   loc_u_view_matrix;
GLint   loc_u_model_matrix;
GLint   loc_u_normal_matrix;

GLint   loc_u_camera_position;
GLint   loc_u_light_position;

GLint   loc_u_light_ambient;
GLint   loc_u_light_diffuse;
GLint   loc_u_light_specular;

GLint   loc_u_obj_ambient;
GLint   loc_u_obj_diffuse;
GLint   loc_u_obj_specular;
GLint   loc_u_obj_shininess;
```

* `init_shader_program()` 함수를 통해 쉐이더 프로그램을 만들 때, GPU 쪽 uniform 및 attribute 변수의 위치를 다음과 같이 받아 놓는다.

```cpp
  loc_u_light_position     = glGetUniformLocation(program, "u_light_position");
  loc_u_light_ambient      = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse      = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular     = glGetUniformLocation(program, "u_light_specular");

  loc_u_obj_ambient        = glGetUniformLocation(program, "u_obj_ambient");
  loc_u_obj_diffuse        = glGetUniformLocation(program, "u_obj_diffuse");
  loc_u_obj_specular       = glGetUniformLocation(program, "u_obj_specular");
  loc_u_obj_shininess      = glGetUniformLocation(program, "u_obj_shininess");

  loc_u_camera_position    = glGetUniformLocation(program, "u_camera_position");
  loc_u_view_matrix        = glGetUniformLocation(program, "u_view_matrix");
  loc_u_model_matrix       = glGetUniformLocation(program, "u_model_matrix");
  loc_u_normal_matrix      = glGetUniformLocation(program, "u_normal_matrix");
  
  loc_a_normal             = glGetAttribLocation(program, "a_normal");
```

* `gen_gl_buffers()` 함수에서 GPU 측 buffer를 생성한다.

* `set_gl_buffers()` 함수에서 CPU 측 데이터를 GPU 측 buffer에 전송한다. 일반적으로 이 함수는 CPU 측 데이터가 변해서 GPU 측 buffer 데이터가 업데이트 되어야 할 경우만 호출한다. 


* `init_shader_program()` 함수를 통해 쉐이더 프로그램을 만들 때, GPU 쪽 uniform 및 attribute 변수의 위치를 다음과 같이 받아 놓는다.

## `./models/` 디렉토리 내부의 모델 데이터 파일 살펴보기
* `./models/` 디렉토리 내부의 모델 데이터 파일을 살펴본다.
* 이번 실습에서 사용할 모델 데이터는 다음과 같이 구성되어 있다.
    * 각 정점은 xyz 삼차원 위치 데이터를 가지고 있다.
    * 폴리곤은 모두 n개의 정점으로 구성되어 있다.
    * 재질(material) 정보를 가지고 있다.

## Assimp를 이용해 PLY 파일 로딩하기
* `main.cpp` 파일의 `load_model()` 함수에서 Assimp 라이브러리를 이용해 3D 모델 데이터 파일을 로딩하는 코드를 구성한다.
* 이번 실습에서 사용하는 모델 데이터는 임의의 다각형 n-gon으로 구성되어 있다.
* 따라서 다각형을 삼각형으로 쪼개어 구성하면서 모델을 로딩하도록 한다.
* 하나의 데이터 파일은 하나의 모델(`Model`)을 구성하도록 한다.
* 한 모델은 여러 재질(material)로 구성된 부분을 가질 수 있는데, 동일 재질로 구성된 부분을 `Mesh`로 구성한다.
* 즉, 하나의 모델(`Model`)은 여러 `Mesh`로 구성되어 있으며, 각 `Mesh`는 단일 재질(material)로 구성되도록 한다.

### Mesh 구조체 구성하기

```cpp
// triangle mesh
struct Mesh
{
  // raw data from a file
  std::vector<glm::vec3>      positions;      // per-vertex 3D positions (raw data)
  std::vector<glm::vec3>      normals;        // per-vertex 3D normal (size = #vertices)
  std::vector<unsigned int>   tv_indices;     // size = 3 x #triangles

  // position and normal for flat shading
  std::vector<glm::vec3>      tv_positions;       // per triangle-vertex 3D position (size = 3 x #triangles)
  std::vector<glm::vec3>      tv_flat_normals;    // per triangle-vertex flat normal (size = 3 x #triangles)
  std::vector<glm::vec3>      tv_smooth_normals;  // per triangle-vertex smooth normal (size = 3 x #triangles)

  GLuint  position_buffer;    // GPU 메모리에서 vertices_buffer 위치 
  GLuint  normal_buffer;      // GPU 메모리에서 normal_buffer 위치

  Material   material;        // mesh material  
};
```

### Model 구조체 구성하기

```cpp
struct Model 
{
  std::vector<Mesh> meshes;    // a model consisting of triangle meshes
  
  std::string       name;

  glm::vec3         translate = glm::vec3(0.0f);
  glm::quat         quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3         scale = glm::vec3(1.5f);
};
```

### `load_model()` 함수 구성하기
* [Assimp 라이브러리 홈페이지](https://assimp.sourceforge.net/lib_html/index.html)를 참고하여 `load_model()` 함수 내의 코드를 작성한다.
* `updata_mesh_data()` 함수를 작성하여 `Mesh` 구조체 내의 triangle soup 데이터를 계산한다.
* 특히 smooth 쉐이딩과 flat 쉐이딩을 모두 지원할 수 있도록 triangle soup 내부의 각 정점에 대한 normal 데이터를 계산하는 알고리즘을 작성한다. 

### GPU 버퍼 데이터 설정
* `gen_gl_buffers()` 함수와 `set_gl_buffers()` 함수를 작성하여 GPU 쪽의 버퍼 데이터를 설정한다.

## `draw_mesh()` 함수 구성하기
* GPU 쪽의 설정된 position, color 버퍼 데이터를 이용해 메쉬를 렌더링한다.
* Triangle soup 데이터로 설정되어 있으므로 `glDrawArrays()` 함수로 렌더링하는 코드를 작성한다.