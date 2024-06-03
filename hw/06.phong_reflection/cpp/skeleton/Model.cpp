#include "Model.h"

#include <iostream>

glm::mat4 Model::get_model_matrix() const
{
    glm::mat4 mat_model = glm::mat4(1.0f);

    mat_model = mat_model * glm::translate(glm::mat4(1.0f), vec_translate_);
    mat_model = mat_model * glm::mat4_cast(quat_rotate_);
    mat_model = mat_model * glm::scale(glm::mat4(1.0f), vec_scale_);

    return mat_model;
}


void Model::draw(int loc_a_position, int loc_a_normal, int loc_u_ambient, int loc_u_diffuse, int loc_u_specular, int loc_u_shininess)
{
    for (std::size_t i = 0; i < meshes.size(); ++i)
    {
        Mesh& mesh = meshes[i];

        // TODO : send material data to GPU
        // call glUniform3fv(...) 
        //      with loc_u_ambient/diffuse/specular
        //      to send mesh.material.ambient/diffuse/specular data to the GPU side
        // call glUniform1f(...) 
        //      with loc_u_shininess
        //      to send mesh.material.shininess

        mesh.draw(loc_a_position, loc_a_normal);
    }
}

bool Model::load_model(const std::string& _path)
{
    set_name(_path);
    const aiScene* scene = aiImportFile(_path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene == NULL)
        return false;

    aiColor3D tmp;    
    aiString name;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        Mesh mesh;
        mesh = Mesh(scene->mMeshes[i]);
        
        mesh.update_tv_indices();
        mesh.gen_gl_buffers();
        mesh.set_gl_buffers(shading_type);

        int mat_idx = scene->mMeshes[i]->mMaterialIndex;    
        
        scene->mMaterials[mat_idx]->Get(AI_MATKEY_COLOR_AMBIENT, tmp);
        glm::vec3 ambient(tmp[0], tmp[1], tmp[2]);

        scene->mMaterials[mat_idx]->Get(AI_MATKEY_COLOR_DIFFUSE, tmp);
        glm::vec3 diffuse(tmp[0], tmp[1], tmp[2]);

        scene->mMaterials[mat_idx]->Get(AI_MATKEY_COLOR_SPECULAR, tmp);
        glm::vec3 specular(tmp[0], tmp[1], tmp[2]);

        Material mat(ambient, diffuse, specular, 5.0f) ;
        scene->mMaterials[mat_idx]->Get(AI_MATKEY_NAME, name);
        mat.name = name.C_Str();

        mesh.set_material(mat);

        meshes.push_back(mesh);        
    }
    return true;
}
