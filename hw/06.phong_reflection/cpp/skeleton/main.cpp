﻿///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <cassert>
#include <map>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// include imGui and imGuIZMO
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imGuIZMOquat.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Light.h"

////////////////////////////////////////////////////////////////////////////////
/// initialization 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* createWindow(int width, int height, const char* title);
void init_window(GLFWwindow* window);
bool init_scene(const std::string& filename);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// shading 관련 변수
//////////////////////////////////////////////////////////////////////////////// 
glm::vec3 g_clear_color(0.5f, 0.5f, 0.5f);
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치
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
GLint   loc_a_normal;

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
std::vector<Camera> g_cameras;
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
std::vector<Model> g_models;
Light g_light;

bool load_asset(const std::string& filename);
// void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
void render(GLFWwindow* window);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// IMGUI / keyboard / scroll input 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
ImFont* font = NULL;
std::string font_path = "../../../../third_party/fonts/NotoSansKR/NotoSansKR-Regular.otf";
float font_size_pixels = 18.0f;
bool is_font_loaded = false;

int g_obj_select_idx = 0;
int g_cam_select_idx = 0;

void init_imgui(GLFWwindow* window);
void compose_imgui_frame(GLFWwindow* window, int key, int scancode, int action, int mods);

void key_callback();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double x, double y);
////////////////////////////////////////////////////////////////////////////////


GLFWwindow* createWindow(int width, int height, const char* title)
{
  GLFWwindow* window; // create window

  // Initialize GLFW Library
  if (!glfwInit())
    return NULL;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return NULL;
  }

  // Make the current OpenGL contexts as one in the window
  glfwMakeContextCurrent(window); 

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
  {
    std::cout << "GLEW Init Error!" << std::endl;
  }

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  return window;
}

void scroll_callback(GLFWwindow* window, double x, double y)
{
  float Fovy = g_cameras[g_cam_select_idx].fovy();
  Fovy += y;
  g_cameras[g_cam_select_idx].set_fovy(Fovy);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  glm::vec3 translate = g_models[g_obj_select_idx].get_translate();
  glm::vec3 scale = g_models[g_obj_select_idx].get_scale();
  
  // move left
  if (key == GLFW_KEY_H && action == GLFW_PRESS) 
    translate[0] -= 0.1f;
  // mode right
  if (key == GLFW_KEY_L && action == GLFW_PRESS)
    translate[0] += 0.1f;
  // move up
  if (key == GLFW_KEY_K && action == GLFW_PRESS)
    translate[1] += 0.1f;
  // move down 
  if (key == GLFW_KEY_J && action == GLFW_PRESS)
    translate[1] -= 0.1f;

  // scale
  if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    scale += 0.1f;
  if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    scale -= 0.1f;
  
  g_models[g_obj_select_idx].set_translate(translate);
  g_models[g_obj_select_idx].set_scale(scale);

  // camera extrinsic parameter
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    g_cameras[g_cam_select_idx].move_left(0.1f);
  if (key == GLFW_KEY_D && action ==GLFW_PRESS)
  g_cameras[g_cam_select_idx].move_right(0.1f);
  if (key == GLFW_KEY_W && action == GLFW_PRESS)
    g_cameras[g_cam_select_idx].move_forward(0.1f);
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
    g_cameras[g_cam_select_idx].move_backward(0.1f);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  Camera& camera = g_cameras[g_cam_select_idx];

  camera.set_aspect((float) width / (float) height);
  glViewport(0, 0, width, height);
}


void init_window(GLFWwindow* window) 
{
  init_imgui(window);
  init_shader_program();

  glEnable(GL_DEPTH_TEST);

  // init_buffer_objects();

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetScrollCallback(window, scroll_callback);
}

bool init_scene(const std::string& filename)
{
  std::ifstream fin(filename);
  if (fin.fail()) 
    return false;

  int count;
  fin >> count;
  for (int i = 0; i < count; i++)
  {
    std::string name;
    glm::vec3   vec_scale, vec_translate;

    fin >> name;
    if (!load_asset(name))
    { 
      std::cout << "Failed to load a asset file: " << name << std::endl;
      return -1;
    }

    fin >> vec_scale[0] >> vec_scale[1] >> vec_scale[2] 
        >> vec_translate[0] >> vec_translate[1] >> vec_translate[2];

    g_models[i].set_scale(vec_scale);
    g_models[i].set_translate(vec_translate);
  }

  // init g_cameras
  fin >> count;

  for (int i = 0; i < count; i++)
  {
    float pos_x, pos_y, pos_z;
    float at_x, at_y, at_z;
    float up_x, up_y, up_z;

    fin >> pos_x >> pos_y >> pos_z 
        >> at_x >> at_y >> at_z 
        >> up_x >> up_y >> up_z;

    glm::vec3 vec_cam_pos = glm::vec3(pos_x, pos_y, pos_z);
    glm::vec3 vec_cam_at  = glm::vec3(at_x, at_y, at_z);
    glm::vec3 vec_cam_up  = glm::vec3(up_x, up_y, up_z);

    Camera camera;

    camera.set_pose(vec_cam_pos, vec_cam_at, vec_cam_up);
    camera.set_mode(Camera::kPerspective);
    camera.set_fovy(60.0f);

    g_cameras.push_back(camera);
  }

  return true;
}

void init_imgui(GLFWwindow* window) 
{
  const char* glsl_version = "#version 120";

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  font = io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size_pixels, NULL, io.Fonts->GetGlyphRangesKorean());
}


bool load_asset(const std::string& filename)
{  
  Model model;
  if (model.load_model(filename))
  {
    g_models.push_back(model);
    return true;
  }
  else 
    return false;
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
    ImGui::Begin("모델(model)");

    for (std::size_t i = 0; i < g_models.size(); i++)
    {
      ImGui::RadioButton(g_models[i].get_name().c_str(), &g_obj_select_idx, i);
    }

    Model& model = g_models[g_obj_select_idx];

    glm::vec3 translate = model.get_translate();
    glm::quat quat;   model.get_rotate(quat);
    glm::vec3 scale = model.get_scale();

    ImGui::SliderFloat3("Tranlsate", glm::value_ptr(translate), -10.0f, 10.0f);
    ImGui::gizmo3D("Rotation", quat);
    ImGui::SliderFloat3("Scale", glm::value_ptr(scale), 0.1f, 2.0f);

    model.set_translate(translate);
    model.set_rotate(quat);
    model.set_scale(scale);

    
    ImGui::NewLine();

    ImGui::Text("Shading");
    ShadingType prev_shading_type = model.shading_type;
    bool is_flat_shading = prev_shading_type ? kFlat : kSmooth;
    ImGui::Checkbox("Flat shading: ", &is_flat_shading);
    model.shading_type = is_flat_shading ? ShadingType::kFlat : ShadingType::kSmooth;
    if (model.shading_type != prev_shading_type)
    {
      for (std::size_t i = 0; i < model.meshes.size(); ++i)
      {
        Mesh& mesh = model.meshes[i];

        mesh.set_gl_buffers(model.shading_type);
      }
      std::cout << "shading changed" << std::endl;
    }    
    ImGui::NewLine();

    ImGui::Text("Materials");

   
    std::string label;
    for (std::size_t i = 0; i < model.meshes.size(); ++i)
    {
      Mesh& mesh = model.meshes[i];

      label = mesh.material.name + "-ambient";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mesh.material.ambient));
      label = mesh.material.name + "-diffuse";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mesh.material.diffuse));
      label = mesh.material.name + "-specular";
      ImGui::ColorEdit3(label.c_str(), glm::value_ptr(mesh.material.specular));
      label = mesh.material.name + "-shininess";
      ImGui::SliderFloat(label.c_str(), &mesh.material.shininess, 0.0f, 500.0f);
      ImGui::NewLine();
    }
    
    ImGui::End();
  }

  // control window
  {
    ImGui::Begin("1인칭 시점 카메라");

    ImGui::RadioButton("Camera 0", &g_cam_select_idx, 0);
    ImGui::RadioButton("Camera 1", &g_cam_select_idx, 1);

    Camera& camera = g_cameras[g_cam_select_idx];

    ImGui::Text("Intrinsic Parameters");
    bool is_perspective = camera.mode() == Camera::kPerspective ? true : false;
    ImGui::Checkbox("Perspective mode: ", &is_perspective);
    camera.set_mode(is_perspective ? Camera::kPerspective : Camera::kOrtho);

    if (camera.mode() == Camera::kPerspective)
    {
      float fovy = camera.fovy();
      ImGui::SliderFloat("fovy (deg)", &fovy, 10.f, 160.f);
      camera.set_fovy(fovy);
    }
    else
    {
      float ortho_scale = camera.ortho_scale();
      ImGui::SliderFloat("ortho zoom", &ortho_scale, 0.1f, 10.f);
      camera.set_ortho_scale(ortho_scale);
    }
    ImGui::NewLine();

    ImGui::Text("Extrinsic Parameters");

    glm::quat   quat_cam;
    glm::vec3   vec_cam_pos;

    camera.get_pose(quat_cam, vec_cam_pos);
    ImGui::SliderFloat3("Tranlsate", glm::value_ptr(vec_cam_pos), -10.0f, 10.0f);    
    ImGui::gizmo3D("Rotation", quat_cam);
    camera.set_pose(quat_cam, vec_cam_pos);
    
    ImGui::End();
  }

  {
    ImGui::Begin("light");

    ImGui::ColorEdit3("background color", glm::value_ptr(g_clear_color));

    glm::vec3 vec(-g_light.pos); 
    ImGui::gizmo3D("Light direction", vec);
    g_light.pos = -vec;

    ImGui::ColorEdit3("ambient light", glm::value_ptr(g_light.ambient));
    ImGui::ColorEdit3("diffuse light", glm::value_ptr(g_light.diffuse));
    ImGui::ColorEdit3("specular light", glm::value_ptr(g_light.specular));

    ImGui::End();
  }

  if (is_font_loaded)
  {
    ImGui::PopFont();
    is_font_loaded = false;
  }
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

  // TODO: get locations of the GPU uniform/attribute variables 
  //       for implementing Phong reflection model

}

void render_object()
{
  Camera& camera = g_cameras[g_cam_select_idx];

  // set transform
  glm::mat4 mat_view = camera.get_view_matrix();
  glm::mat4 mat_proj = camera.get_projection_matrix();


  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // TODO : send uniform for camera & light to GPU

  for (std::size_t i = 0; i < g_models.size(); ++i)
  {
    Model& model = g_models[i];

    // TODO : set mat_model, mat_normal, mat_PVM 
    // TODO : send uniform data for model to GPU
    
    model.draw(loc_a_position, loc_a_normal, loc_u_obj_ambient, loc_u_obj_diffuse, loc_u_obj_specular, loc_u_obj_shininess);
  }

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

void render(GLFWwindow* window) 
{
  glClearColor(g_clear_color[0], g_clear_color[1], g_clear_color[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwPollEvents();
  compose_imgui_frame();

  render_object();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
  //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
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


int main(int argc, char* argv[])
{
  // create window
  GLFWwindow* window = createWindow(1000, 1000, "Hello Assimp");

  // initialize window
  init_window(window);
  if (!init_scene("info.txt"))
  {
    std::cout << "Failed to load a info file" << std::endl;
    return -1;
  }

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    render(window);
  }

  glfwTerminate();

  return 0;
}
