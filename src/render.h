#if !defined(RENDER_H)
#define RENDER_H

#include "maths.h"
#include "image.h"

struct screen_space_effect;
static const int min_z_buffer_val = -1000;

struct output_buffers{
    image frame_buffer;
    image temp_buffer;
    float * z_buffer{};
};

/*
 * Due to how this app is structured, I did not bother writing a "free output buffers"
 * function. The memory is needed for as long as the program is running and when
 * it closes I can rely on the OS to clean up the allocated data for me. 
 */
void init_output_buffers(output_buffers& output_buffers, int width, int height);
void clear_output_buffers(output_buffers& output_buffers, const rgba& clear_color);

struct render_state{
    v3 eye{};
    v3 center{};
    v3 up{};
    v3 light_dir{};

    m4 model_view{};
    m4 projection{};
    m4 viewport{};

    output_buffers output_buffers;

    bool backspace_culling = true;
    bool wire_frame = false;
    bool smooth_shading = true;

    float dt=0;
    float culm_dt=0;
};


struct model;
struct mesh;
struct ui_state;
struct shader
{
    render_state * renderer_state{};
    mesh * mesh_to_draw{};
    model* model_to_draw{};
    
    virtual const char* name() = 0;
    virtual void begin_pass() = 0;
    virtual v4 vertex(v3 & vertex, int face_no, int vert_no) = 0;
    virtual bool fragment(const v3& bar, rgba & col, v3 interpolated_normal, v2 interpolated_uv, const v2_i& screen_pos) = 0;

    shader() = default;

    /* Prevent any accidental copying */
    shader& operator = (shader && rhs) = delete;
    shader& operator = (shader & rhs) = delete;
    shader(const shader & rhs) = delete;
    shader(shader&&) = delete;
    
    protected:
        virtual ~shader() = default;
};

struct screen_space_effect
{
    render_state* renderer_state{};
    image* temp_buffer{};

    virtual const char* name() = 0;
    virtual rgba apply(image* frame_buffer, int x, int y, const rgba& pixel) = 0;
    virtual void render_ui(v2_i& base_pos, ui_state& ui_state, output_buffers& output) = 0;
    virtual void reset_settings() = 0;

    screen_space_effect() = default;

    /* Prevent any accidental copying */
    screen_space_effect(const screen_space_effect & rhs) = delete;
    screen_space_effect(screen_space_effect&&) = delete;
    screen_space_effect& operator =(screen_space_effect && rhs) = delete;
    screen_space_effect& operator =(screen_space_effect & rhs) = delete;
    
    protected:
        virtual ~screen_space_effect() = default;

};

void draw_model(model & obj, render_state& state, shader& shader);
void draw_line(v2_i v0, v2_i v1, image& out, rgba col);
void apply_screen_space_effect(screen_space_effect& effect, render_state & state);

#endif