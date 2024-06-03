#include "Camera.h"

// TODO: fill up the following function properly 
void Camera::set_rotation(const glm::quat& _q)
{

}

// TODO: re-write the following function properly 
const glm::quat Camera::get_rotation() const
{
  return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::quat& _q, const glm::vec3& _t)
{

}

// TODO: fill up the following function properly 
void Camera::get_pose(glm::quat& _q, glm::vec3& _t) const
{

}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_pose() const
{
  return  glm::mat4(1.0f);
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::mat4& _frame)
{
  // +x_cam: right direction of the camera    (it should be a unit vector whose length is 1)
  // right_dir_ = ..... ; // +x
  // +y_cam: up direction of the camera       (it should be a unit vector whose length is 1)   
  // up_dir_    = ..... ;    // +y
  // -z_cam: front direction of the camera    (it should be a unit vector whose length is 1)
  // front_dir_ = ..... ;    // -z
  // pos_cam: position of the camera
  // position_  = ..... ;    // pos
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir)
{
  // up_dir_    = ..... ;
  // front_dir_ = ..... ;    // -z_cam direction
  // right_dir_ = ..... ;    // +x_cam direction
  // up_dir_    = ..... ;    // +y_cam direction

  // position_  = ..... ;      // pos
}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_view_matrix() const
{
  return glm::mat4(1.0f);
}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_projection_matrix() const
{
  // TODO: Considering the followings,
  //       You must return a proper projection matrix
  //       i) camera mode: it can be either kOrtho or kPerspective
  //       ii) zoom-in/-out: if the camera mode is kOrtho, 
  //                         utilize ortho_scale_ for zoom-in/-out
  //                         if the camera mode is kPerspective,
  //                         utilize fovy_ for zoom-in/-out
  //       iii) aspect ratio: utilize aspect_ in the both camera modes
  //       iv) near/far clipping planes: utilize near_, far_

  return glm::mat4(1.0f);
}

// TODO: fill up the following functions properly 
void Camera::move_forward(float delta)
{

}

// TODO: fill up the following functions properly 
void Camera::move_backward(float delta)
{

}

// TODO: fill up the following functions properly 
void Camera::move_left(float delta)
{

}

// TODO: fill up the following functions properly 
void Camera::move_right(float delta)
{

}

// TODO: fill up the following functions properly 
void Camera::move_up(float delta)
{

}

// TODO: fill up the following functions properly 
void Camera::move_down(float delta)
{

}
