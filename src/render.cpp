#include "render.h"
#include "file.h"

void init_output_buffers(output_buffers & output_buffers)
{
	const auto size = output_buffers.frame_buffer.width * output_buffers.frame_buffer.height * output_buffers.frame_buffer.n_channels;
	
    output_buffers.frame_buffer.data = new unsigned char[size];
    memset(output_buffers.frame_buffer.data, 0, size);

    output_buffers.temp_buffer.data = new unsigned char[size];
    output_buffers.temp_buffer.height = output_buffers.frame_buffer.height;
    output_buffers.temp_buffer.width = output_buffers.frame_buffer.width;
    output_buffers.temp_buffer.n_channels = output_buffers.frame_buffer.n_channels;
    memset(output_buffers.temp_buffer.data, 0, size);
	
    const auto z_buffer_size = output_buffers.frame_buffer.width * output_buffers.frame_buffer.height;

    output_buffers.z_buffer = new float[z_buffer_size];
    for (auto i = 0; i < output_buffers.frame_buffer.width * output_buffers.frame_buffer.height; i++)
    {
        output_buffers.z_buffer[i] = min_z_buffer_val;
    }
}

void clear_output_buffers(output_buffers& output_buffers, const rgba& clear_color)
{
    auto& frame_buffer = output_buffers.frame_buffer;
	auto* z_buffer = output_buffers.z_buffer;

    for (auto i = 0; i < frame_buffer.width * frame_buffer.height; i++)
    {
        z_buffer[i] = min_z_buffer_val;
    } 

    auto* walk = reinterpret_cast<rgba*>(frame_buffer.data);
    for(auto i = 0; i < frame_buffer.width * frame_buffer.height; i++)
    {
        *walk++ = clear_color;
    }
}

void draw_line(v2_i v0, v2_i v1, image & out, rgba col){
	const auto distance_x = abs(v1.x - v0.x);
	const auto stride_x = v0.x < v1.x ? 1 : -1;
	const auto distance_y = -abs(v1.y - v0.y);
	const auto stride_y = v0.y < v1.y ? 1 : -1;

	auto error = distance_x + distance_y;

	const auto max_iterations = 10000;
    for(auto iteration = 0; iteration < max_iterations; iteration++){
        if(
            v0.x >= 0 && v0.x < out.width - 1 &&
            v0.y >= 0 && v0.y < out.height - 1
        ){
            set_pixel(out, col, v0.x, v0.y);
        }
		
        if(v0.x == v1.x && v0.y == v1.y) break;

        const auto twice_error = 2 * error;

        if(twice_error >= distance_y){
            error += distance_y;
            v0.x +=  static_cast<int>(stride_x);
        }

        if(twice_error <= distance_x){
            error += distance_x;
            v0.y += static_cast<int>(stride_y);
        }
    }
}

v3 barycentric(const v2_i & p0, const v2_i & p1, const v2_i & p2, const v2_i & p) {
	const auto u = cross(
        v3{ static_cast<float>(p2.x-p0.x), static_cast<float>(p1.x-p0.x), static_cast<float>(p0.x -p.x) },
        v3{ static_cast<float>(p2.y-p0.y), static_cast<float>(p1.y-p0.y), static_cast<float>(p0.y -p.y) }
    );

    if (std::abs(u.z) < 1){
        return v3{-1,1,1};
    } 

    return v3{1.0f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z}; 
}

inline int r_min(int x, int y, int z){
    if(x > y) std::swap(x,y);
    if(x > z) std::swap(x,z);
    return x;
}

inline int r_max(int x, int y, int z){
    if(x < y) std::swap(x,y);
    if(x < z) std::swap(x,z);
    return x;
}

inline int clamp(int val, const int min, const int max){
    if(val < min) val = min;
    if(val > max) val = max;
    return val;
}

void triangle(
    v4 vtx0, v4 vtx1, v4 vtx2,
    v2 uv0, v2 uv1, v2 uv2, 
    v3 n0, v3 n1, v3 n2,
    v3 tri_normal,
    output_buffers & output_buffers,
    render_state & state,
    shader & shader
){
	auto vtx0_screen = state.viewport * vtx0;
	auto vtx1_screen = state.viewport * vtx1;
	auto vtx2_screen = state.viewport * vtx2;

	auto t0 = v3_to_v2(project_3d(vtx0_screen));
    auto t1 = v3_to_v2(project_3d(vtx1_screen));
    auto t2 = v3_to_v2(project_3d(vtx2_screen));
	
	auto min_x = clamp(r_min(t0.x, t1.x, t2.x), 0, output_buffers.frame_buffer.width - 1);
    auto max_x = clamp(r_max(t0.x, t1.x, t2.x), 0, output_buffers.frame_buffer.width - 1);
    assert(min_x <= max_x);

	auto min_y = clamp(r_min(t0.y, t1.y, t2.y), 0, output_buffers.frame_buffer.height - 1);
	auto max_y = clamp(r_max(t0.y, t1.y, t2.y), 0, output_buffers.frame_buffer.height - 1);
	assert(min_y <= max_y);

    for(auto y = min_y; y <= max_y; y++){
        for(auto x = min_x; x <= max_x; x++){
	        auto bc = barycentric(t0, t1, t2, v2_i{x, y});
        	
            //draw point if inside triangle
            if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0){
                //interpolate z using barycentric coordinates
                auto z = static_cast<float>(vtx0.z) * bc.x +
							  static_cast<float>(vtx1.z) * bc.y +
							  static_cast<float>(vtx2.z) * bc.z;

            	//get current z buffer value
                auto* memloc = &output_buffers.z_buffer[
                    static_cast<int>(x + (output_buffers.frame_buffer.height - y) * output_buffers.frame_buffer.width)
                ];
            	
            	//only render the pixel if we are closer to the camera then the current z buffer value
                if(*memloc < z){
                    *memloc = z;

                    //pass clip space barycentric coordinates to get perspective accurate texture mapping 
                    auto clip_space_bc = v3{ bc.x / vtx0.w, bc.y / vtx1.w, bc.z / vtx2.w, };
                    clip_space_bc = clip_space_bc / (clip_space_bc.x + clip_space_bc.y + clip_space_bc.z);

                    v2 screen_pos{ static_cast<float>(x), static_cast<float>(y) };
                    auto interpolated_uv = uv0 * clip_space_bc.x + uv1 * clip_space_bc.y + uv2 * clip_space_bc.z;
                    v3 interpolated_normal{};
                    
                    if(state.smooth_shading){
                        interpolated_normal = (n0 * clip_space_bc.x + n1 * clip_space_bc.y + n2 * clip_space_bc.z).normalise();
                    }
                    else{
                        interpolated_normal = tri_normal;
                    }

                    rgba col{};
                    if(shader.fragment(clip_space_bc, col, interpolated_normal, interpolated_uv, screen_pos)){
                        set_pixel(output_buffers.frame_buffer, col, x, y);
                    }
                }
            }
        }
    }

	//draw triangle wireframe if wireframe is on
    if (state.wire_frame) {
        draw_line(t0, t1, output_buffers.frame_buffer, orange);
        draw_line(t1, t2, output_buffers.frame_buffer, orange);
        draw_line(t2, t0, output_buffers.frame_buffer, orange);
    }
}

void draw_model(model & obj, render_state & state, shader & shader)
{
    shader.model_to_draw = &obj;
    shader.renderer_state = &state;

	//viewer position in object space, used for fast backface culling
    const auto view_position_object_space = m4_to_m3(state.projection * state.model_view).invert() * state.eye;

	for(size_t i = 0; i < obj.mesh_count; i++)
	{
        auto& mesh = obj.meshes[i];
        shader.mesh_to_draw = &mesh;
        shader.begin_pass();
		
        for (size_t face_no = 0; face_no < mesh.face_count; face_no++) {
            auto& face = mesh.faces[face_no];

            //calculate triangle normal
            auto normal = cross(
                mesh.verts[face.verts.y] - mesh.verts[face.verts.x],
                mesh.verts[face.verts.z] - mesh.verts[face.verts.x]
            ).normalise();

            //cull the triangle if it is back facing
            if (
                state.backspace_culling &&
                normal.inner(mesh.verts[face.verts.x] - view_position_object_space) >= 0
            )
            {
                continue;
            }

            //run the vertex shader
            v4 tri[3];
            for (auto vert_no = 0; vert_no < 3; vert_no++) {
                tri[vert_no] = shader.vertex(mesh.verts[face.verts.e[vert_no]], face_no, vert_no);
            }

            //rasterize the triangle
            triangle(
                tri[0], tri[1], tri[2],
                mesh.uvs[face.uv.x],
                mesh.uvs[face.uv.y],
                mesh.uvs[face.uv.z],
                mesh.normals[face.normal.x],
                mesh.normals[face.normal.y],
                mesh.normals[face.normal.z],
                normal,
                state.output_buffers,
                state,
                shader
            );
        }
	}
}


void apply_screen_space_effect(screen_space_effect& effect, render_state& state)
{
    effect.temp_buffer = &state.output_buffers.temp_buffer;
    effect.renderer_state = &state;

    const auto width = state.output_buffers.frame_buffer.width;
    const auto height = state.output_buffers.frame_buffer.height;

    auto* const img_data = reinterpret_cast<rgba*>(state.output_buffers.frame_buffer.data);

	//run the effect on a temporary buffer
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            const auto idx = y * width + x;

            auto pixel = img_data[idx];

            const auto new_pixel = effect.apply(&state.output_buffers.frame_buffer, x, y, pixel);

            reinterpret_cast<rgba*>(state.output_buffers.temp_buffer.data)[idx] = new_pixel;
        }
    }

	//copy effect output to the framebuffer
	for (auto i = 0; i < width * height; i++)
	{
        img_data[i] = reinterpret_cast<rgba*>(state.output_buffers.temp_buffer.data)[i];
	}
}