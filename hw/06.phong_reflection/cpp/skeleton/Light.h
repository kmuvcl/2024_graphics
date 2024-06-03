#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

class Light
{  
public:
    Light() :
        ambient(0.0f),
        diffuse(1.0f),
        specular(0.5f) {}
public:
    glm::vec3   ambient;        // ambient light (r, g, b)
    glm::vec3   diffuse;        // diffuse light (r, g, b)
    glm::vec3   specular;        // specular light (r, g, b)
    glm::vec3   pos;        // position of light source
};