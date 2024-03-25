///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  color_buffer;     // GPU 메모리에서 color_buffer의 위치
GLuint  index_buffer;     // GPU 메모리에서 index_buffer의 위치

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
// triangle-vetex indices
GLubyte g_indices[] = {
  0, 1, 2, 0, 3, 1,
};

// per-vertex 3D positions (x, y, z)
GLfloat g_position[] = {
  0.5f,  0.5f,  0.0f,          // 0th vertex position
  -0.5f, -0.5f,  0.0f,          // 1st vertex position
  0.5f, -0.5f,  0.0f,          // 2nd vertex position
  -0.5f, 0.5f,  0.0f,          // 3rd vertex position
};

// per-vertex RGB color (r, g, b)
GLfloat g_color[] = {
  1.0f, 0.0f, 0.0f,             // 0th vertex color (red)
  0.0f, 1.0f, 0.0f,             // 1st vertex color (green)
  0.0f, 0.0f, 1.0f,             // 2nd vertex color (blue)
  1.0f, 1.0f, 0.0f,             // 3rd vertex color (yellow)
};

void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////


// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  // Get rid of BOM in the head of shader_string
  // Because, some GLSL compiler (e.g., Mesa Shader compiler) cannot handle UTF-8 with BOM
  if (shader_string.compare(0, 3, "\xEF\xBB\xBF") == 0)  // Is the file marked as UTF-8?
  {
    std::cout << "Shader code (" << filename << ") is written in UTF-8 with BOM" << std::endl;
    std::cout << "  When we pass the shader code to GLSL compiler, we temporarily get rid of BOM" << std::endl;
    shader_string.erase(0, 3);                  // Now get rid of the BOM.
  }

  const GLchar* shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar * *)& shader_src, NULL);
  glCompileShader(shader);

  GLint is_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled != GL_TRUE)
  {
    std::cout << "Shader COMPILE error: " << std::endl;

    GLint buf_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetShaderInfoLog(shader, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
  if (is_linked != GL_TRUE)
  {
    std::cout << "Shader LINK error: " << std::endl;

    GLint buf_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetProgramInfoLog(program, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteProgram(program);
    program = 0;
  }

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color = glGetAttribLocation(program, "a_color");
}

void init_buffer_objects()
{
  /////////////////////////////////////////////////////////////////////
  /// CPU 메모리에서 GPU 메모리로 물체의 데이터를 전송하는 부분 - BEGIN
  /////////////////////////////////////////////////////////////////////
  
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_position), g_position, GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color), g_color, GL_STATIC_DRAW);

  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_indices), g_indices, GL_STATIC_DRAW);
  /////////////////////////////////////////////////////////////////////
  /// CPU 메모리에서 GPU 메모리로 물체의 데이터를 전송하는 부분 - END
  /////////////////////////////////////////////////////////////////////
}


// object rendering
void render_object()
{
  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  /////////////////////////////////////////////////////////////////////
  /// TODO: GPU에 이미 전송된 데이터로 물체를 그리는 부분 - BEGIN
  /////////////////////////////////////////////////////////////////////

  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 버텍스 쉐이더의 attribute 중 a_position 부분 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_position에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 color_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  // 버텍스 쉐이더의 attribute 중 a_color 부분 활성화
  glEnableVertexAttribArray(loc_a_color);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_color에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_color, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  
  // 삼각형 그리기
  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, g_indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (void*)0);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);

  /////////////////////////////////////////////////////////////////////
  /// TODO: GPU에 이미 전송된 데이터로 물체를 그리는 부분 - END
  /////////////////////////////////////////////////////////////////////

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}


int main(void)
{
  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Hello Triangle", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  // Make the current OpenGL context as one in the window
  glfwMakeContextCurrent(window);

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
    std::cout << "GLEW Init Error!" << std::endl;

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  // Init OpenGL
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


  init_shader_program();

  // TODO: GPU의 VBO를 초기화하는 함수 호출
  init_buffer_objects();

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: 물체(삼각형)를 렌더링하는 함수 호출
    render_object();

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
