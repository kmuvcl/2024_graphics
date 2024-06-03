#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h> 
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


#include <GL/glew.h>
#include <iostream>

#include "ShadingType.h"
#include "Material.h"


class Mesh
{
public:

public:
    Mesh() {};
    Mesh(const aiMesh* _pmesh) : pmesh_(_pmesh) {}

    void gen_gl_buffers();    
    void set_gl_buffers(ShadingType shading_type);
    void update_tv_indices();
    
    void draw(int loc_a_position, int loc_a_normal); 
    void print_info();

    void set_material(const Material& _mat) { material = _mat; }
    
    Material    material;

protected:

    void set_gl_position_buffer_();
    void set_gl_color_buffer_(unsigned int cs_idx);
    void set_gl_normal_buffer_(ShadingType shading_type);

private:
    GLuint  position_buffer_;   // GPU 메모리에서 vertices_buffer 위치 
    GLuint  color_buffer_;      // GPU 메모리에서 color_buffer 위치
    GLuint  normal_buffer_;     // GPU 메모리에서 normal_buffer 위치   
    bool    is_color_ = false;

    // std::vector<Face> faces;
    std::vector<unsigned int>   tv_indices_;
    
    const aiMesh* pmesh_;
};
