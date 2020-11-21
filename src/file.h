#ifndef FILE_H
#define FILE_H

#include "maths.h"
#include "image.h"
#include "render.h"

struct face
{
    v3_i verts;
    v3_i uv;
    v3_i normal;
};

struct mesh
{    
    image diffuse;
    image normal;
    image spec;
    image emission;

    bool allow_lighting{};
    bool has_emissive_map{};
    bool has_normal_map{};
    bool has_specular_map{};

    const char* geo_path{};
    const char* diffuse_path{};
    const char* normal_path{};
    const char* specular_path{};
    const char* emission_path{};

    size_t vert_count{};
    size_t normal_count{};
    size_t uv_count{};
    size_t face_count{};
    
    v3 * verts{};
    v3 * normals{};
    v2 * uvs{};
    face * faces{};
};

struct model
{
    hsla background{};
    rgba text_col{};
    v3 initial_rot{};

    mesh * meshes{};
    size_t mesh_count{};
    const char * author{};
    const char * name{};
    const char * url{};

    inline int get_face_count() const;
};


void load_models(const char* path, model*& output, int& model_count);

#endif