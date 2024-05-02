///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// include imGui and imGuIZMO
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imGuIZMOquat.h"

// model hpp
#include "models/avocado_vlist.hpp"

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
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
glm::mat4   g_mat_model, g_mat_view, g_mat_proj;

// model transformation
glm::vec3   g_vec_model_translate;  // T
glm::quat   g_quat_model_rotation;  // R, ImGuIZMO.quat 관련 변수
glm::vec3   g_vec_model_scale;      // S

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수
////////////////////////////////////////////////////////////////////////////////
float   g_fovy;     // fovy in degree
float   g_aspect;   // aspect ratio
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 장면(scene) 초기화 함수
////////////////////////////////////////////////////////////////////////////////
void init_scene();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////

void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// IMGUI / keyboard input 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
ImFont* font = NULL;
std::string font_path = "../../../../third_party/fonts/NotoSansKR/NotoSansKR-Regular.otf";
float font_size_pixels = 18.0f;
bool is_font_loaded = false;

void init_imgui(GLFWwindow* window);
void compose_imgui_frame(GLFWwindow* window, int key, int scancode, int action, int mods);

void key_callback();
////////////////////////////////////////////////////////////////////////////////


void init_imgui(GLFWwindow* window) 
{
  const char* glsl_version = "#version 120";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size_pixels, NULL, io.Fonts->GetGlyphRangesKorean());
}

void compose_imgui_frame()
{
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  if (!is_font_loaded)
  {
    ImGui::PushFont(font);
    is_font_loaded = true;
  }

  // control window
  {
    ImGui::Begin("콘트롤(control)");

    // TODO
    ImGui::SliderFloat("translate", &g_vec_model_translate[0], -3.0f, 3.0f);

    ImGui::End();
  }

  // output window
  {
    ImGui::Begin("출력(output)");
    
    ImGui::TextColored(ImVec4(0,1,1,1), "Model");
    ImGui::TextColored(ImVec4(0,1,1,1), "----------");

    ImGui::TextColored(ImVec4(1,1,0,1), "Translate");
    ImGui::Text("x = %.3f, y = %.3f, z = %.3f", g_vec_model_translate[0], g_vec_model_translate[1], g_vec_model_translate[2]);

    glm::mat3 mat_rot = glm::mat3_cast(g_quat_model_rotation);
    ImGui::TextColored(ImVec4(1,1,0,1), "Rotation matrix");    
    ImGui::Text("%.3f %.3f %.3f ", mat_rot[0][0], mat_rot[1][0], mat_rot[2][0]);
    ImGui::Text("%.3f %.3f %.3f ", mat_rot[0][1], mat_rot[1][1], mat_rot[2][1]);
    ImGui::Text("%.3f %.3f %.3f ", mat_rot[0][2], mat_rot[1][2], mat_rot[2][2]);

    ImGui::TextColored(ImVec4(1,1,0,1),"Scaling");
    ImGui::Text("x = %.3f, y = %.3f, z = %.3f", g_vec_model_scale[0], g_vec_model_scale[1], g_vec_model_scale[2]);
    ImGui::NewLine();

    ImGui::TextColored(ImVec4(1,1,0,1), "model matrix");
    ImGui::Text("%.3f %.3f %.3f %.3f ", g_mat_model[0][0], g_mat_model[1][0], g_mat_model[2][0], g_mat_model[3][0]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", g_mat_model[0][1], g_mat_model[1][1], g_mat_model[2][1], g_mat_model[3][1]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", g_mat_model[0][2], g_mat_model[1][2], g_mat_model[2][2], g_mat_model[3][2]);
    ImGui::Text("%.3f %.3f %.3f %.3f ", g_mat_model[0][3], g_mat_model[1][3], g_mat_model[2][3], g_mat_model[3][3]);
    ImGui::NewLine();

    ImGui::End();
  }

  if (is_font_loaded)
  {
    ImGui::PopFont();
    is_font_loaded = false;
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // move left
  if (key == GLFW_KEY_H && action == GLFW_PRESS) 
    g_vec_model_translate[0] -= 0.1f;
  // mode right
  if (key == GLFW_KEY_L && action == GLFW_PRESS)
    g_vec_model_translate[0] += 0.1f;
  
  // TODO

  // scale
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    g_vec_model_scale += 0.1f;
  
  // TODO
}


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
  // VBO
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(avocado::vlist::position), avocado::vlist::position, GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(avocado::vlist::color), avocado::vlist::color, GL_STATIC_DRAW);

  // IBO 
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(avocado::vlist::index), avocado::vlist::index, GL_STATIC_DRAW);
}

void set_transform() 
{
  // view matrix
  g_mat_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), 
                           glm::vec3(0.0f, 0.0f, 0.0f), 
                           glm::vec3(0.0f, 1.0f, 0.0f));
  // projection matrix
  g_mat_proj = glm::perspective(glm::radians(g_fovy), g_aspect, 0.001f, 1000.f);
  
  // TODO: erase the following line and write your codes to properly set g_mat_model as T*R*S
  g_mat_model = glm::translate(g_vec_model_translate);
}


void render_object()
{
  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  glm::mat4 mat_PVM = g_mat_proj * g_mat_view * g_mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, glm::value_ptr(mat_PVM));

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

  // IBO를 이용해 물체 그리기
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glDrawElements(GL_TRIANGLES, avocado::vlist::num_index, GL_UNSIGNED_INT, (void*)0);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

void init_scene()
{
  // model transformation
  g_vec_model_translate = glm::vec3(0.0);
  g_vec_model_scale = glm::vec3(1.f);

  // TODO: initialize quaternion for model rotation
  // g_quat_model_rotation = ...

  g_fovy = 60.0f;
  g_aspect = 1.0f;
}

int main(void)
{
  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(800, 800, "User Interaction", NULL, NULL);
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

  init_imgui(window);
  init_shader_program();

  init_buffer_objects();

  glfwSetKeyCallback(window, key_callback);

  init_scene();

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable (GL_DEPTH_TEST);

    glfwPollEvents();
    compose_imgui_frame();

    set_transform();
    render_object();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
