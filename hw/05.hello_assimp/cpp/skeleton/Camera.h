#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
  enum Mode { kOrtho, kPerspective };

public:
  Camera()  { }

  void move_forward(float delta);
  void move_backward(float delta);
  void move_left(float delta);
  void move_right(float delta);
  void move_up(float delta);
  void move_down(float delta);
	
  const glm::vec3  position() const         { return  position_; }
  void  set_position(const glm::vec3& _p)   { position_ = _p; }

  const glm::vec3  front_direction() const  { return  front_dir_; } 
  const glm::vec3  up_direction() const     { return  up_dir_; }
  const glm::vec3  right_direction() const  { return  right_dir_; }

  const glm::mat4  get_pose() const;
  void  set_pose(const glm::mat4& _frame);

  void  get_pose(glm::quat& _q, glm::vec3& _t) const;
  void  set_pose(const glm::quat& _q, const glm::vec3& _t);

  void  set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir);

  // camera to world rotation
  // i.e., 1st, 2nd, 3rd columns are +x_cam, +y_cam, +z_cam axes 
  // in the world coordinate system, respectively
  void  set_rotation(const glm::quat& _q);
  const glm::quat  get_rotation() const;

  const glm::mat4  get_view_matrix() const;
  const glm::mat4  get_projection_matrix() const;

  const float fovy() const                   { return fovy_; }
  void  set_fovy(float _fovy)                { fovy_ = _fovy; }

  const float ortho_scale() const            { return ortho_scale_; }
  void  set_ortho_scale(float _ortho_scale)  { ortho_scale_ = _ortho_scale; }

  const float aspect() const                 { return aspect_; }
  void  set_aspect(float _aspect)            { aspect_ = _aspect; }

  Camera::Mode mode() const                  { return mode_; }
  void  set_mode(Camera::Mode _mode)         { mode_ = _mode; }

private:
	// position of the camera
  glm::vec3  position_ = glm::vec3(0.0f, 0.0f, 0.5f);
  // front direction of the camera    (it should be a unit vector whose length is 1)      
  glm::vec3  front_dir_ = glm::vec3(0.0f, 0.0f, -1.0f);
  // up direction of the camera       (it should be a unit vector whose length is 1)   
  glm::vec3  up_dir_ = glm::vec3(0.0f, 1.0f, 0.0f);
  // right direction of the camera    (it should be a unit vector whose length is 1)
  glm::vec3  right_dir_ = glm::vec3(1.0f, 0.0f, 0.0f);

  Mode  mode_ = kOrtho;

  float ortho_scale_ = 1.0f;
  float fovy_ = 60.f;     // degree

  float aspect_ = 1.0f;

  float near_ = 0.01f;
  float far_  = 1000.0f;
};