# TODO list

## Scene 파일 살펴보기
* `info.txt` 파일은 이번 실습에서 사용하는 간단한 장면 파일이다.
* 장면 파일은 모델 정보와 카메라 정보로 구성되어 있다.
* 편의상 이번 실습에서는 1개의 모델, 1개의 카메라로만 구성된 장면 파일만 다루도록 한다.
* `main.cpp` 파일의 `init_scene()` 함수에서 장면 파일을 로딩하는 코드를 살펴보도록 하자.

## PLY 파일 살펴보기
* `./models/` 디렉토리 아래에 있는 `*.ply` 파일을 살펴본다.
* PLY 파일은 헤더 부분에 파일 구성이 요약되어 있다.
* 이번 실습에서 사용할 `*.ply` 파일은 다음과 같이 구성되어 있다.
    * 각 정점 별로 xyz 삼차원 위치와 rgba 컬러 데이터로 구성되어 있다.
    * 폴리곤은 모두 3개의 정점으로 구성되어 있다.

## Assimp를 이용해 PLY 파일 로딩하기
* `main.cpp` 파일의 `load_model()` 함수에서 Assimp 라이브러리를 이용해 PLY 파일을 로딩하는 코드를 구성한다.

### Mesh 구조체 구성하기
* PLY 파일을 로딩하여 CPU 메모리에 올릴 때 필요한 자료구조를 `Mesh` 구성하도록 하자.
* 이번 실습에서 사용할 `*.ply` 파일의 내용에 따라 `Mesh` 구조체는 구성하자.

```cpp
struct Mesh
{
  // raw data from a file
  std::vector<glm::vec3>      positions;      // per-vertex 3D positions (raw data)
  std::vector<glm::vec4>      colors;         // per-vertex rgba colors (raw data)
  std::vector<unsigned int>   tv_indices;     // size = 3 x #triangles

  // position and colors for triangle-vertices
  std::vector<glm::vec3>      tv_positions;   // per triangle-vertex 3D position (size = 3 x #triangles)
  std::vector<glm::vec4>      tv_colors;      // per triangle-vertex rgba (size = 3 x #triangles)

  GLuint  position_buffer;    // GPU 메모리에서 vertices_buffer 위치 
  GLuint  color_buffer;       // GPU 메모리에서 color_buffer 위치
};
```

### `load_model()` 함수 구성하기
* [Assimp 라이브러리 홈페이지](https://assimp.sourceforge.net/lib_html/index.html)를 참고하여 `load_model()` 함수 내의 코드를 작성한다.
* `updata_mesh_data()` 함수를 작성하여 `Mesh` 구조체 내의 triangle soup 데이터를 계산한다.

### GPU 버퍼 데이터 설정
* `gen_gl_buffers()` 함수와 `set_gl_buffers()` 함수를 작성하여 GPU 쪽의 position, color 버퍼 데이터를 설정한다.

## `draw_mesh()` 함수 구성하기
* GPU 쪽의 설정된 position, color 버퍼 데이터를 이용해 메쉬를 렌더링한다.
* Triangle soup 데이터로 설정되어 있으므로 `glDrawArrays()` 함수로 렌더링하는 코드를 작성한다.