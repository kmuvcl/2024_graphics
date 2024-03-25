#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

class Light
{  
public:
    Light() {}
public:
    glm::vec3   ambient = glm::vec3(0.0f, 0.0f, 0.0f);        // ambient light (r, g, b)
    glm::vec3   diffuse = glm::vec3(1.0f, 1.0f, 1.0f);        // diffuse light (r, g, b)
    glm::vec3   specular = glm::vec3(0.5f, 0.5f, 0.5f);       // specular light (r, g, b)
    glm::vec3   pos = glm::vec3(1.0f, 1.0f, 1.0f);            // position of light source
};