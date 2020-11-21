#include <cstdio>
#include <cstring>

#include "file.h"
#include "platform_specific.h"

static void concat_strings(
    const size_t source_a_count, const char *source_a,
    const size_t source_b_count, const char *source_b,
    char *dest
)
{
    for(size_t i = 0; i < source_a_count; ++i)
    {
        *dest++ = *source_a++;
    }

    for(size_t i = 0; i < source_b_count; ++i)
    {
        *dest++ = *source_b++;
    }

    //final null character to terminate string
    *dest = 0;
}

inline int model::get_face_count() const
{
    auto sum = 0;

    for(size_t i = 0; i < this->mesh_count; i++)
    {
        sum += this->meshes[i].face_count;
    }    

    return sum;
}

static size_t read_int_checked(FILE* f)
{
    auto val = 0;
    const auto num_read = fread(&val, sizeof(val), 1, f);
    assert(num_read == 1);

    return val;
}

static bool read_bool_checked(FILE* f)
{
    auto val = false;
    const auto num_read = fread(&val, sizeof(val), 1, f);
    assert(num_read == 1);

    return val;
}

static const char * read_string_checked(FILE* f)
{
    const auto string_len = read_int_checked(f);
    if(string_len == 0) return nullptr;

    const char * ret = new char[string_len];

    const auto num_read = fread((void*) ret, sizeof(char),string_len, f);
    assert(num_read == string_len);

    return ret;
}

static rgba read_color_checked(FILE* f)
{
    rgba col{};

    const auto num_read = fread(&col, sizeof(col), 1, f);
    assert(num_read == 1);

    return col;
}

static hsla read_hsla_checked(FILE* f)
{
    hsla col{};

    const auto num_read = fread(&col, sizeof(col), 1, f);
    assert(num_read == 1);

    return col;
}


static v3 read_v3_checked(FILE* f)
{
    v3 v{};

    const auto num_read = fread(&v, sizeof(v), 1, f);
    assert(num_read == 1);

    return v;
}

void read_mesh(const char* path, mesh& out)
{
    FILE * f = nullptr;
    open_binary_file(path, f);
    assert(f != nullptr);

    //load verts
    {
        const auto vert_count = read_int_checked(f); 
    
        auto* verts = new v3[vert_count];
        assert(verts != nullptr);
        
        const auto num_read = fread(verts, sizeof(v3), vert_count, f);
        assert(num_read == vert_count);

        out.verts = verts;
        out.vert_count = vert_count;
    }
    
    //load faces
    {
        const auto face_count = read_int_checked(f); 
    
        auto* faces = new face[face_count];
        assert(faces != nullptr);

        const auto num_read = fread(faces, sizeof(face), face_count, f);
        assert(num_read == face_count);
        
        out.faces = faces;
        out.face_count = face_count;
    }
    
    //load uvs
    {
        const auto uv_count = read_int_checked(f); 

        auto* uvs = new v2[uv_count];
        assert(uvs != nullptr);
        
        const auto num_read = fread(uvs, sizeof(v2), uv_count, f);
        assert(num_read == uv_count);
        
        out.uvs = uvs;
        out.uv_count = uv_count;
    }
    
    //load normals
    {
        const auto normal_count = read_int_checked(f); 

        auto* normals = new v3[normal_count];
        assert(normals != nullptr);

        const auto num_read = fread(normals, sizeof(v3), normal_count, f);
        assert(num_read == normal_count);
        
        out.normals = normals;
        out.normal_count = normal_count;
    }
    
    fclose(f);

    printf(
        "Loaded Bin: V:%u F:%u UV:%u N:%u\n",
        static_cast<unsigned>(out.vert_count),
        static_cast<unsigned>(out.face_count),
        static_cast<unsigned>(out.uv_count),
        static_cast<unsigned>(out.normal_count)
    );
}


void load_models(const char* path, model* & output, int& model_count)
{
    FILE * f = nullptr;
    open_binary_file(path, f);
    assert(f != nullptr);
 
    //read model count
    model_count = read_int_checked(f);

    output = new model[model_count];
    assert(output != nullptr);

    for (auto i = 0; i < model_count; i++)
    {
        auto& model = output[i];

        //read author, name, url
        model.author = read_string_checked(f);
        model.name = read_string_checked(f);
        model.url =  read_string_checked(f);

        //read colors
        model.background = read_hsla_checked(f);
        model.text_col = read_color_checked(f);

        //read starting rotation
        model.initial_rot = read_v3_checked(f);

        //read mesh count
        const auto mesh_count = read_int_checked(f);
        model.mesh_count = mesh_count;

        model.meshes = new mesh[mesh_count];
        assert(model.meshes != nullptr);
        
        for (size_t j = 0; j < mesh_count; j++)
        {
            auto& mesh = model.meshes[j];

            //read lighting flag
            mesh.allow_lighting = read_bool_checked(f);

            //read file paths
            mesh.geo_path = read_string_checked(f);
            assert(mesh.geo_path != nullptr);

            mesh.diffuse_path = read_string_checked(f);
            assert(mesh.diffuse_path != nullptr);

            //these can be null
            mesh.normal_path = read_string_checked(f);
            mesh.has_normal_map = mesh.normal_path != nullptr && strlen(mesh.normal_path) > 0;

            mesh.specular_path = read_string_checked(f);
            mesh.has_specular_map = mesh.specular_path != nullptr && strlen(mesh.normal_path) > 0;

            mesh.emission_path = read_string_checked(f);
            mesh.has_emissive_map = mesh.emission_path != nullptr && strlen(mesh.normal_path) > 0;

            //load the referenced resources
            char model_bin_path[1024];
            concat_strings( strlen(mesh.geo_path), mesh.geo_path, strlen(".bin"), ".bin", model_bin_path);
            
            read_mesh(model_bin_path, mesh);
            load_image(mesh.diffuse_path, mesh.diffuse);

            if(mesh.has_normal_map)
            {
                load_image(mesh.normal_path, mesh.normal);
            }

            if(mesh.has_specular_map)
            {
                load_image(mesh.specular_path, mesh.spec);
            }

            if(mesh.has_emissive_map)
            {
                load_image(mesh.emission_path, mesh.emission);
            }
        }
    }

    fclose(f);
}
