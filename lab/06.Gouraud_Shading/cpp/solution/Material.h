#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

class Material
{
public:
    Material() {}
    Material(
        const glm::vec3& _ambient,
        const glm::vec3& _diffuse,
        const glm::vec3& _specular,
        float     _shininess
    ) : ambient(_ambient), diffuse(_diffuse), specular(_specular), shininess(_shininess) {}

public:
    std::string name;           // material name

    glm::vec3   ambient = glm::vec3(0.1f, 0.1f, 0.1f);  // ambient reflection coefficients k_a (r, g, b)
    glm::vec3   diffuse = glm::vec3(1.0f, 1.0f, 1.0f);  // diffuse reflection coefficients k_d (r, g, b)
    glm::vec3   specular = glm::vec3(1.0f, 1.0f, 1.0f); // specular reflection coefficients k_s (r, g, b)
    float       shininess = 5.0f;   // shininess term
};
