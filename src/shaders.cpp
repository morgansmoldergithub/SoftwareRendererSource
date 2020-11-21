#include <cmath>

#include "render.h"
#include "file.h"
#include "ui.h"

v2_i get_tex_indicies(const v2& uv, const mesh& mesh)
{
    return {
        static_cast<int>(uv.x * static_cast<float>(mesh.diffuse.width - 1)),
        static_cast<int>(uv.y * static_cast<float>(mesh.diffuse.height - 1))
    };
}

struct blinn_shader_normal_map final : public shader{
    m4 model_view_proj{};
    m3 normal_mat{};

    //outputs for fragment
    v3 ndc_vertex[3]{};
    v2 vertex_uv[3]{};

    const char* name() override { return "Blinn Normal Map"; }

    void begin_pass() override
    {
        normal_mat = (m4_to_m3(renderer_state->projection * renderer_state->model_view)).invert().transpose();
        
        model_view_proj = renderer_state->projection * renderer_state->model_view;
    }

    v4 vertex(v3 & vertex, int face_no, int vert_no) override
    {
        const auto ret = model_view_proj  * project_4d(vertex);

        if(mesh_to_draw -> allow_lighting && mesh_to_draw->has_normal_map){
            ndc_vertex[vert_no] = project_3d(renderer_state->projection * renderer_state->model_view * project_4d(vertex));
            vertex_uv[vert_no] = mesh_to_draw->uvs[mesh_to_draw->faces[face_no].uv.e[vert_no]];
        }

        return ret;
    }



    bool fragment(const v3& bar, rgba & col, v3 interpolated_normal, v2 interpolated_uv, const v2_i& screen_pos) override
    {
        const auto tex_indicies = get_tex_indicies(interpolated_uv, *mesh_to_draw);
        
        auto dif = get_pixel(mesh_to_draw->diffuse, tex_indicies.x, tex_indicies.y);
        col = dif;

        //skip lighting calculations
        if(!mesh_to_draw->allow_lighting) return true;

        v3 normal{};

        //sample the normal map if we have one
        if (mesh_to_draw->has_normal_map) {
            interpolated_normal = normal_mat * interpolated_normal;

            //calculate tangent and bitangent for pixel 
            auto ai = m3{ ndc_vertex[1] - ndc_vertex[0], ndc_vertex[2] - ndc_vertex[0], interpolated_normal }.invert();

            v3 u_diff{ vertex_uv[1].x - vertex_uv[0].x, vertex_uv[2].x - vertex_uv[0].x, 0 };
            auto i = (ai * u_diff).normalise();

            v3 v_diff{ vertex_uv[1].y - vertex_uv[0].y, vertex_uv[2].y - vertex_uv[0].y, 0 };
            auto j = (ai * v_diff).normalise();

            auto b = m3{ i, j, interpolated_normal }.transpose();

            normal = get_normal(mesh_to_draw->normal, tex_indicies.x, tex_indicies.y);

            normal = (b * normal).normalise();
        }
        //otherwise use the passed normal
        else
        {
            normal = normal_mat * interpolated_normal;
        }
        
        //lighting
        auto l = renderer_state->light_dir;

        auto diffuse = normal.inner( l );
        if (diffuse < 0) diffuse = 0;

        float spec = 0;
        if(mesh_to_draw->has_specular_map)
        {
            auto spec_rgb = get_pixel(mesh_to_draw->spec, tex_indicies.x, tex_indicies.y);

            auto r = (normal * (normal.inner(l)) * 2 - l).normalise();
            if (r.z < 0) {
                r.z = 0;
            }
            
            spec = pow(r.z, 5 + spec_rgb.b);
        }
        
        col = col * (1.2f * diffuse + 0.6f * spec);

        //add in ambient color
        col = col + dif * 0.15f;

        return true;
    }
};

struct flat_shader final : public shader{
    m3 normal_mat{};
    m4 model_view_proj{};
    v3 l{};
    v3 view_dir{};

    float raise_factor{};

    const char* name() override { return "Flat"; }
    
    void begin_pass() override
    {
        l = (m4_to_m3(renderer_state->model_view) * renderer_state->light_dir).normalise();
        normal_mat = (m4_to_m3(renderer_state->projection * renderer_state->model_view)).invert().transpose();
        model_view_proj = renderer_state->projection * renderer_state->model_view;
        view_dir = renderer_state->eye - renderer_state->center;
    }

    v4 vertex(v3 & vertex, int face_no, int vert_no) override
    {
        return model_view_proj  * project_4d(vertex);
    }
    
    bool fragment(const v3& bar, rgba& col, v3 interpolated_normal, v2 interpolated_uv, const v2_i& screen_pos) override
    {
        auto normal = normal_mat * interpolated_normal;
        const auto spec = 1 - (normal * (normal.inner(l)) * 2 - l).normalise().z;

        if(spec > 0.5f)
        {
            auto hsl = model_to_draw->background;
            hsl.l -= 0.3f;
            hsl.s -= .4f;

            //complimentary hue
            hsl.h += .5;
            if(hsl.h > 1) hsl.h -= 1;

            col = hsl_to_rgb(hsl);

            return true;
        }

        //get grid effect by not drawing every fourth pixel
        if (static_cast<int>(screen_pos.x) % 4 == 0 || static_cast<int>(screen_pos.y) % 4 == 0)
        {
            col = hsl_to_rgb( model_to_draw->background);

            return true;
        } 

        auto col_final = model_to_draw->background;

        col_final.l -= 0.1f;

        col_final.h += .5;
        if(col_final.h > 1) col_final.h -= 1;


        col = hsl_to_rgb(col_final);

        return true;
    }
};

struct chromatic_aberration final : public screen_space_effect
{
    int red_offset = 1;
    int green_offset = -1;
    int blue_offset = -2;

    static rgba sample_rgb_safe(int x, int y, image* buf)
    {
        if (x > buf->width - 1) x = buf->width - 1;
        if (x < 0) x = 0;
        if (y > buf->height - 1) y = buf->height - 1;
        if (y < 0) y = 0;

        return get_pixel(*buf, x, y);
    }

    const char* name() override { return "Chromatic Aberration"; }

    rgba apply(image* frame_buffer, int x, int y, const rgba& pixel) override
    {
        rgba res{};

        y = frame_buffer->height - 1 - y;

        res.r = sample_rgb_safe(x + red_offset, y + red_offset, frame_buffer).r;
        res.g = sample_rgb_safe(x + green_offset, y + green_offset, frame_buffer).g;
        res.b = sample_rgb_safe(x + blue_offset, y + blue_offset, frame_buffer).b;

        return  res;
    }

    void render_ui(v2_i& base_pos, ui_state& ui_state, output_buffers& output) override
    {
        int_selector(base_pos, output, ui_state, red_offset, 1);
        increment_col(base_pos, ui_state);
        blit_string(base_pos, "Red Offset", ui_state, output, ui_state.text_col);
        increment_row(base_pos, ui_state);

        int_selector(base_pos, output, ui_state, blue_offset, 1);
        increment_col(base_pos, ui_state);
        blit_string(base_pos, "Blue Offset", ui_state, output, ui_state.text_col);
        increment_row(base_pos, ui_state);

        int_selector(base_pos, output, ui_state, green_offset, 1);
        increment_col(base_pos, ui_state);
        blit_string(base_pos, "Green Offset", ui_state, output, ui_state.text_col);
        increment_row(base_pos, ui_state);
    }
    
    void reset_settings() override
    {
        red_offset = 1;
        green_offset = -1;
        blue_offset = -2;
    }
};

inline float pixel_to_greyscale_float(const rgba& pixel)
{
    return static_cast<float>(pixel.r + pixel.g + pixel.b) / (255.0f * 3.0f);
}

struct sobel_filter final : public screen_space_effect
{
    const m3 gx{
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
    };

    const m3 gy{
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };

    float threshold = 0.2f;
    
    const char* name() override { return "Sobel Filter"; }

    rgba apply(image* frame_buffer, int x, int y, const rgba& pixel) override
    {
        //only apply the shader to pixels where we have rendered something.
        const auto z_val = renderer_state->output_buffers.z_buffer[y * renderer_state->output_buffers.frame_buffer.width + x];
        if(!(z_val > min_z_buffer_val))
        {
            return pixel;
        }
        
        //can't filter edge pixels of image (need 3x3 matrix of surrounding pixels)
        if (
            x == 0 || x >= frame_buffer->width - 2 ||
            y == 0 || y >= frame_buffer->height - 2
        )
        {
            return pixel;
        }

        const auto width = frame_buffer->width;

        const rgba* data_rgb = reinterpret_cast<rgba*>(frame_buffer->data);

        const m3 a{
            pixel_to_greyscale_float(data_rgb[(y - 1) * width + x - 1]),
            pixel_to_greyscale_float(data_rgb[(y - 1) * width + x]),
            pixel_to_greyscale_float(data_rgb[(y - 1) * width + x + 1]),

            pixel_to_greyscale_float(data_rgb[y * width + x - 1]),
            pixel_to_greyscale_float(data_rgb[y * width + x]),
            pixel_to_greyscale_float(data_rgb[y * width + x + 1]),
        
            pixel_to_greyscale_float(data_rgb[(y + 1) * width + x - 1]),
            pixel_to_greyscale_float(data_rgb[(y + 1) * width + x]),
            pixel_to_greyscale_float(data_rgb[(y + 1) * width + x + 1]),
        };
        
        const auto s1 = (gx * a).sum();
        const auto s2 = (gy * a).sum();

        const auto res = abs(s1) + abs(s2);
        
        if (res < threshold) {
            return rgba{ 15, 15, 15, 15 };
        }
        
        return rgba{
            static_cast<unsigned char>(res * 255), 
            static_cast<unsigned char>(res * 255),
            static_cast<unsigned char>(res * 255),
        };

    }

    void render_ui(v2_i& base_pos, ui_state& ui_state, output_buffers& output) override
    {
        float_selector(base_pos, output, ui_state, threshold, 0.1f);
        increment_col(base_pos, ui_state);
        blit_string(base_pos, "Threshold", ui_state, output, ui_state.text_col);
        increment_row(base_pos, ui_state);
    }
    
    void reset_settings() override
    {
        threshold = 0.2f;
    }
};

struct jumbo_pixels final : public screen_space_effect
{
    int pixel_size = 1;

    const char* name() override { return "Jumbo Pixels"; }
    
    rgba apply(image* frame_buffer, int x, int y, const rgba& pixel) override
    {
        //only apply the shader to pixels where we have rendered something.
        const auto z_val = renderer_state->output_buffers.z_buffer[y * renderer_state->output_buffers.frame_buffer.width + x];
        if (!(z_val > min_z_buffer_val))
        {
            return pixel;
        }
        
        x = x % (3 * pixel_size);
        
        if (x < pixel_size) {
            return rgba{ pixel.r };
        }
        if (x < 2 * pixel_size) {
            return rgba{ 0, pixel.g };
        }
        
        return rgba{ 0, 0, pixel.b };
    }

    void render_ui(v2_i& base_pos, ui_state& ui_state, output_buffers& output) override
    {
        int_selector(base_pos, output, ui_state, pixel_size, 1);
        increment_col(base_pos, ui_state);
        blit_string(base_pos, "Pixel Size", ui_state, output, ui_state.text_col);
        increment_row(base_pos, ui_state);

        //don't let pixel size be less then or equal to zero
        if (pixel_size < 1) {
            pixel_size = 1;
        }
    }

    void reset_settings() override
    {
        pixel_size = 1;
    }
};
