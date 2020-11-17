#include <cassert>
#include <chrono> 
#include <cstdio>
#include <vector>

#define SDL_MAIN_HANDLED 
#include "./include/SDL/SDL.h"

/*
    Unity build
*/
#include "platform_specific.cpp"
#include "maths.cpp"
#include "image.cpp"
#include "file.cpp"
#include "render.cpp"
#include "shaders.cpp"
#include "ui.cpp"

/*
    App Shaders
*/
static blinn_shader_normal_map blinn_shader_normal_map;
static rim_shader rim_shader;
static flat_shader flat_shader;

static const int shader_count = 3;
static shader * shaders[shader_count] = {
    &blinn_shader_normal_map,
    &flat_shader,
    &rim_shader,
};

/*
    Screen Space Effects
*/
static jumbo_pixels jumbo_pixels;
static chromatic_aberration chromatic_aberration;
static sobel_filter sobel_filter;
static const int screen_space_effect_count = 3;
static screen_space_effect* screen_space_effects[screen_space_effect_count] = {
	&chromatic_aberration,
    & sobel_filter,
	&jumbo_pixels,
};

/*
    Model Buffer
*/
static int model_count;
static model * models;

struct application_state
{
    render_state gl_state;
    ui_state ui_state;

    model* active_model{};
    int active_model_idx{};

    shader* active_shader{};
    int active_shader_idx{};

    screen_space_effect* active_effect{};
    int active_effect_idx{};

    bool running{};
    bool use_fx = false;

    //active model rotation/transform
    v3 target_rot{};
    v3 target_trans{};

    //framebuffer clear color & lerp data
    hsla background_color{};
    hsla target_background_color{};
    float background_lerp_percent{};

	//true if the app will sleep in one frame.
    bool impending_sleep{};
};

SDL_Window* global_window = nullptr;
SDL_Surface* global_screen_surface = nullptr;
application_state global_app_state;

static int main_loop(application_state & app_state, SDL_Window* window, SDL_Surface* screen_surface);

//emscripten main loop
#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>

EM_BOOL one_iter(double time, void* userData) {
    main_loop(global_app_state, global_window, global_screen_surface);
	
    // Return true to keep the loop running.
    return EM_TRUE;
}
#endif

int main(int argc, char* args[]) {
    /* Load font texture */
    image letter_sampler{};
    assert(load_image("./obj/courier_new.png", letter_sampler));

    /* Load the models */
    load_models("./obj/conf.bin", models, model_count);

    /* Initialise output buffers. The width/height values specified here determine rendering resolution */
    output_buffers main_output{ 512, 512 };
    printf("Rendering with Width:%d and Height:%d\n", main_output.frame_buffer.width, main_output.frame_buffer.height);

    init_output_buffers(main_output);

    /* Initialise application settings */
    global_app_state = application_state{
    	//renderer state
        {
            v3{ 0, 0, 3 },
            v3{ 0, 0, 0 },
            v3{ 0, 1, 0 },
            v3{ 1, 1, 1 }.normalise(),
            identity(),
            projection(v3{ 0, 0, 3 }, v3{ 0, 0, 0 }),
            view_port(
                static_cast<float>(main_output.frame_buffer.width) / 10,
                static_cast<float>(main_output.frame_buffer.height) / 10,
                static_cast<float>(main_output.frame_buffer.width) * 4 / 5,
                static_cast<float>(main_output.frame_buffer.height) * 4 / 5
            ),
            main_output
        },
        //ui state
        {
            //Font image
            letter_sampler,
        },
        //active model
        &models[0], 0,
        //active shader
        shaders[0], 0,
        //active screen space effect
        screen_space_effects[0], 0,
    };

    global_app_state.target_rot = global_app_state.active_model->initial_rot;

    global_app_state.background_lerp_percent = 1;
    global_app_state.background_color = global_app_state.active_model->background;
    global_app_state.target_background_color = global_app_state.background_color;

    clear_output_buffers(global_app_state.gl_state.output_buffers, hsl_to_rgb(global_app_state.background_color));
    

    const auto& frame_buffer = global_app_state.gl_state.output_buffers.frame_buffer;

    /* Initialise SDL and begin main loop */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
    else
    {
        global_window = SDL_CreateWindow(
            "Software Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            frame_buffer.width, frame_buffer.height, 0
        );
        if (global_window == nullptr)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            global_screen_surface = SDL_GetWindowSurface(global_window);

            #ifdef EMSCRIPTEN
            
            //webgl main loop
            emscripten_request_animation_frame_loop(one_iter, 0);

            #else

            //desktop main loop
            global_app_state.running = true;
            while (global_app_state.running) {
                main_loop(global_app_state, global_window, global_screen_surface);
            }
            
            SDL_Quit();

            #endif
        }
    }

    return 0;
}

static void draw_ui(application_state & app_state);
static void poll_events(application_state& app_state);
static void update_application_colors(application_state& app_state);
static void draw_scene(application_state & app_state, SDL_Surface* screen_surface);
static void copy_frame_buffer_to_screen(application_state& app_state, SDL_Surface* screen_surface);

static int main_loop(application_state & app_state, SDL_Window* window, SDL_Surface* screen_surface)
{
	const auto start = std::chrono::high_resolution_clock::now(); 

	//get user input
	poll_events(app_state);

    //mouse isn't over the screen, user can't interact with it;
    //so lets not waste time rendering.
    if (!app_state.ui_state.mouse_in_window && app_state.impending_sleep) {
        return 0;
    }
    if(!app_state.ui_state.mouse_in_window)
    {
    	//provide a single clear frame before sleeping, to render an image
    	//without the ui in the way
        app_state.impending_sleep = true;
    }

	//update the application color theme
    update_application_colors(app_state);
	
	//render the scene
	draw_scene(app_state, screen_surface);

	//apply active screen space effect if enabled
    if(app_state.use_fx)
    {
        apply_screen_space_effect(*app_state.active_effect, app_state.gl_state);
    }

    //render the user interface if we aren't about to sleep
    if (!app_state.impending_sleep)
    {
        draw_ui(app_state);
    }
	
	//blit the render to the window
	copy_frame_buffer_to_screen(app_state, screen_surface);

	//Update the window
	SDL_UpdateWindowSurface( window );

	//clear the output buffers for next frame
    clear_output_buffers(app_state.gl_state.output_buffers, hsl_to_rgb(app_state.background_color));

    //calculate and store delta time
	const auto stop = std::chrono::high_resolution_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	const auto frame_duration_seconds = duration.count() / 1000.0f;

    app_state.gl_state.dt = frame_duration_seconds;
    app_state.gl_state.culm_dt += frame_duration_seconds;

	return 0;
}

static void poll_events(application_state & app_state)
{
    SDL_GetMouseState(&app_state.ui_state.mouse_x, &app_state.ui_state.mouse_y);
    //invert mouse y - want bottom left to be window origin (matches output buffers)
    app_state.ui_state.mouse_y = app_state.gl_state.output_buffers.frame_buffer.height - app_state.ui_state.mouse_y;

    //reset mouse down state for this frame
    app_state.ui_state.mouse_down_this_frame = false;

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_QUIT:{
                app_state.running = false;
            } break;

	        case SDL_MOUSEMOTION: {
	            if (app_state.ui_state.mouse_down) {
                    app_state.target_rot.x += e.motion.yrel;
	                app_state.target_rot.y += e.motion.xrel;
	            }
	        } break;

	        case SDL_MOUSEBUTTONDOWN: {
	            if (e.button.button == SDL_BUTTON_LEFT) {
	                app_state.ui_state.mouse_down = true;
                    app_state.ui_state.mouse_down_this_frame = true;
                }
	        } break;

	        case SDL_MOUSEBUTTONUP: {
	            if (e.button.button == SDL_BUTTON_LEFT) {
	                app_state.ui_state.mouse_down = false;
	            }
	        } break;

	        default: ;
        }

        switch (e.window.event)
        {
            case SDL_WINDOWEVENT_CLOSE:
                app_state.running = false;
                                        
            case SDL_WINDOWEVENT_ENTER:
                app_state.ui_state.mouse_in_window = true;
                app_state.impending_sleep = false;
                break;

            case SDL_WINDOWEVENT_LEAVE:
                app_state.ui_state.mouse_in_window = false;
                app_state.impending_sleep = false;
        		break; 
            default:
                break;
        }
    }
}

static void draw_scene(application_state & app_state, SDL_Surface* screen_surface)
{
	//apply model transform
    app_state.gl_state.model_view = look_at(
        app_state.gl_state.eye,
        app_state.gl_state.center,
        app_state.gl_state.up
    )
      * rot_x(app_state.target_rot.x)
      * rot_y(app_state.target_rot.y)
      * trans(app_state.target_trans);

	//render the model
    draw_model(*app_state.active_model, app_state.gl_state, *app_state.active_shader);
}

static int alter_idx_wrapped(int index, const int mod_amount, const int max)
{
    index = (index + mod_amount) % max;

	if(index < 0)
	{
        index = max - 1;
	}

    return index;
}

static void draw_ui(application_state& app_state)
{
	auto& output = app_state.gl_state.output_buffers;
    auto& ui_state = app_state.ui_state;

	//start at the bottom left of the screen
	auto ui_draw_position = ui_state.screen_margin;
    ui_state.row_start_x = ui_state.screen_margin.x;

    //draw upward
    ui_state.draw_rows_up = true;
	
	//Use FX toggle
    labeled_toggle(ui_draw_position, ui_state, output, "Use FX", app_state.use_fx);

	//wireframe toggle
    labeled_toggle(ui_draw_position, ui_state, output, "Wireframe", app_state.gl_state.wire_frame);

	//smooth shading toggle
    labeled_toggle(ui_draw_position, ui_state, output, "Smooth Shading", app_state.gl_state.smooth_shading);

	//model selection
    {
        auto model_left = false, model_right = false;
        left_right_selector(ui_draw_position, ui_state, output, "Model", model_left, model_right);

        if (model_left || model_right)
        {
	        const auto new_idx = alter_idx_wrapped(app_state.active_model_idx, model_left ? -1 : 1, model_count);
            
            app_state.active_model_idx = new_idx;
        	
            app_state.active_model = &models[app_state.active_model_idx];
            app_state.ui_state.text_col = app_state.active_model->text_col;

            //setup background color lerp
            app_state.target_background_color = app_state.active_model->background;
            app_state.background_lerp_percent = 0;

            //reset rotation to model configured rotation
            app_state.target_rot = app_state.active_model->initial_rot;
        }
    }

	//shader selection
    {
        auto shader_left = false, shader_right = false;
        left_right_selector(ui_draw_position, ui_state, output, "Shader", shader_left, shader_right);

        if (shader_left || shader_right)
        {
            const auto new_idx = alter_idx_wrapped(app_state.active_shader_idx, shader_left ? -1 : 1, shader_count);

        	app_state.active_shader_idx = new_idx;

            app_state.active_shader = shaders[app_state.active_shader_idx];
        }
    }

	//draw shader and effect ui at the bottom left of the screen
    ui_state.row_start_x = output.frame_buffer.width - 270;
    ui_draw_position = v2_i{ ui_state.row_start_x, ui_state.screen_margin.y };

	//render the effects ui if effects are active
	if(app_state.use_fx)
	{
        //effect selection
        {
            auto effect_left = false, effect_right = false;
            left_right_selector(ui_draw_position, ui_state, output, "Effect", effect_left, effect_right);

            if (effect_left || effect_right)
            {
                const auto new_idx = alter_idx_wrapped(app_state.active_effect_idx, effect_left ? -1 : 1, screen_space_effect_count);

                app_state.active_effect_idx = new_idx;

                app_state.active_effect = screen_space_effects[app_state.active_effect_idx];
            }
        }

        labeled_string(ui_draw_position, ui_state, output, "Effect:", app_state.active_effect->name());

        if (labeled_button(ui_draw_position, ui_state, output, "Reset Effect"))
        {
            app_state.active_effect->reset_settings();
        }
		
        app_state.active_effect->render_ui(ui_draw_position, ui_state, output);
	}
    
	//switch to drawing just below top of screen
    ui_draw_position = v2_i{ ui_state.screen_margin.x, output.frame_buffer.height - ui_state.screen_margin.y * 2 };
    ui_state.row_start_x = ui_state.screen_margin.x;

    //draw downward
    ui_state.draw_rows_up = false;

	//draw performance information
    char buf[1024];
    FORMAT_PRINT(buf, "%.3f", 1024, app_state.gl_state.dt);
    labeled_string(ui_draw_position, ui_state, output, "Frame MS:", buf);

    //draw tri count
    FORMAT_PRINT(buf, "%d", 1024, app_state.active_model->get_face_count());
    labeled_string(ui_draw_position, ui_state, output, "Triangles:", buf);

    //draw shader/model information
    ui_draw_position.y -= 5;
    labeled_string(ui_draw_position, ui_state, output, "Shader:", app_state.active_shader->name());

	//draw model author information
    ui_draw_position.y -= 5;
    labeled_string(ui_draw_position, ui_state, output, "Model:", app_state.active_model->name);
    labeled_string(ui_draw_position, ui_state, output, "Author:", app_state.active_model->author);
    #ifdef EMSCRIPTEN
    if (labeled_button(ui_draw_position, ui_state, output, "Visit Model URL"))
    {
        EM_ASM_({
			window.open(Module.UTF8ToString($0), '_blank');
        }, app_state.active_model->url);
    }
    #endif
}

static float lerp_float(const float start, const float end, float percent)
{
    if (percent > 1) percent = 1;

    return start + percent * (end - start);
}

static void update_application_colors(application_state& app_state)
{
	//lerp the app background color if we are not at the correct value
    if (app_state.background_lerp_percent < 1.0f) {
        app_state.background_lerp_percent += app_state.gl_state.dt / 3000.0f;

        app_state.background_color.h = lerp_float(
            app_state.background_color.h,
            app_state.target_background_color.h,
            app_state.background_lerp_percent
        );
    }

	//update the ui colors
    auto complimentary_color_light = app_state.background_color;
    //flip the app background color hue to get a complimentary color
    complimentary_color_light.h -= 0.5f;
    if (complimentary_color_light.h > 1) complimentary_color_light.h -= 1;

	//darker hue of complimentary color
    auto complimentary_color_dark = complimentary_color_light;
    
    complimentary_color_dark.l -= .5f;
    complimentary_color_light.l -= .35f;

	const auto complimentary_color_light_rgb = hsl_to_rgb(complimentary_color_light);
	const auto complimentary_color_dark_rgb = hsl_to_rgb(complimentary_color_dark);

	//update ui colors
    app_state.ui_state.button_color = complimentary_color_light_rgb;
    app_state.ui_state.toggle_active_color = complimentary_color_light_rgb;

    app_state.ui_state.button_hover_color = complimentary_color_dark_rgb;
    app_state.ui_state.toggle_hover_color = complimentary_color_dark_rgb;

    //propagate color changes to html
#if EMSCRIPTEN
    auto bg = hsl_to_rgb(app_state.background_color);
    auto text = complimentary_color_dark_rgb;
    EM_ASM_({
        var color_a = 'rgb(' + $0 + ',' + $1 + ',' + $2 + ')';
        var color_b = 'rgb(' + $3 + ',' + $4 + ',' + $5 + ')';

        var links = document.getElementsByTagName('a');
        for (var i = 0; i < links.length; i++)
        {
            links[i].style.color = color_b;
        }

        document.body.style.backgroundColor = color_a;
        }, bg.r, bg.g, bg.b, text.r, text.g, text.b);
#endif
}

struct bit_scan_result
{
    bool found;
    int index;
};

static bit_scan_result find_least_significant_set_bit(const int value)
{
    bit_scan_result result{};

#if COMPILER_MSVC
    result.Found = _BitScanForward(reinterpret_cast<unsigned long*>(&result.Index), Value);
#else
    for (auto test = 0; test < 32; test++)
    {
        if (value & (1 << test))
        {
            result.index = test;
            result.found = true;

            break;
        }
    }
#endif

    return result;
}

static void copy_frame_buffer_to_screen(application_state& app_state, SDL_Surface* screen_surface)
{
	auto& frame_buffer = app_state.gl_state.output_buffers.frame_buffer;
    
    auto* frame_pixels = reinterpret_cast<rgba*>(app_state.gl_state.output_buffers.frame_buffer.data);
    auto* target = static_cast<unsigned int*>(screen_surface->pixels);

    const auto r_shift = find_least_significant_set_bit(screen_surface->format->Rmask);
    const auto b_shift = find_least_significant_set_bit(screen_surface->format->Bmask);
    const auto g_shift = find_least_significant_set_bit(screen_surface->format->Gmask);

    assert(r_shift.found);
    assert(b_shift.found);
    assert(g_shift.found);
	
    for (auto y = 0; y < frame_buffer.height; y++) {
        for (auto x = 0; x < frame_buffer.width; x++) {
	        const auto old_pixel = *frame_pixels++;

            //don't blit alpha, sdl screen surface doesn't let us anyway
            const int r = old_pixel.r;
            const int g = old_pixel.g;
            const int b = old_pixel.b;

            *target++ = r << r_shift.index | g << g_shift.index | b << b_shift.index;
        }
    }
}