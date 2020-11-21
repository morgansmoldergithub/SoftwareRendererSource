#include "ui.h"

inline void increment_row(v2_i& offset, const ui_state& state)
{
    offset = state.draw_rows_up
                ? offset + state.row_padding
                : offset - state.row_padding;

    offset.x = state.row_start_x;
}

inline void increment_col(v2_i& offset, const ui_state& state)
{
    offset = state.draw_cols_right
                ? offset + state.col_padding + v2_i{ state.__last_element_size.x, 0 }
                : offset - state.col_padding - v2_i{ state.__last_element_size.x, 0 };
}

void labeled_toggle(
    v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output,
    const char* label, bool& state
)
{
    toggle(ui_draw_position, output, ui_state, state);
    increment_col(ui_draw_position, ui_state);
    blit_string(ui_draw_position, label, ui_state, output, ui_state.text_col);
    increment_row(ui_draw_position, ui_state);
}

void labeled_string(v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output, const char* label, const char* string)
{
    blit_string(ui_draw_position, label, ui_state, output, ui_state.text_col);
    increment_col(ui_draw_position, ui_state);
    blit_string(ui_draw_position, string, ui_state, output, ui_state.text_col);
    increment_row(ui_draw_position, ui_state);
}

void left_right_selector(
    v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output, 
    const char* label, bool& left_clicked, bool& right_clicked
)
{
    left_clicked = false;
    right_clicked = false;

    const v2_i button_icon_offset{3,0};
    
    if (button(ui_draw_position, output, ui_state))
    {
        left_clicked = true;
    }
    blit_string(ui_draw_position + button_icon_offset, "-", ui_state, output, white);
    increment_col(ui_draw_position, ui_state);
    blit_string(ui_draw_position, label, ui_state, output, ui_state.text_col);
    increment_col(ui_draw_position, ui_state);
    if (button(ui_draw_position, output, ui_state))
    {
        right_clicked = true;
    }
    blit_string(ui_draw_position + button_icon_offset, "+", ui_state, output, white);
    increment_row(ui_draw_position, ui_state);
}

bool labeled_button(
    v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output,
    const char* label
)
{
    const auto result = button(ui_draw_position, output, ui_state);
    increment_col(ui_draw_position, ui_state);
    blit_string(ui_draw_position, label, ui_state, output, ui_state.text_col);
    increment_row(ui_draw_position, ui_state);

    return result;
}

inline void draw_box(const v2_i& min, const v2_i& size, output_buffers& out, const rgba& col){
    const auto max = min + size;

    for(auto y = min.y; y < max.y; y++){
        for(auto x = min.x; x < max.x; x++){
            set_pixel(out.frame_buffer, col, x, y);
        }
    }
}

inline bool intersect_box(const v2_i& min, const v2_i& size, const int x, const int y){
    const auto max = min + size;

    return x > min.x && x < max.x && 
           y > min.y && y < max.y; 
}

bool button(const v2_i& min, output_buffers& out, ui_state& ui_state){
    const auto hovering = intersect_box(min, ui_state.button_size, ui_state.mouse_x, ui_state.mouse_y);
    const auto pressed = hovering && ui_state.mouse_down && ui_state.mouse_down_this_frame;

    //toggle off mouse
    if(pressed){
        ui_state.mouse_down = false;
    }

    const auto target_color = pressed
                                        ? ui_state.button_press_color
                                        : hovering
                                            ? ui_state.button_hover_color
                                            : ui_state.button_color;

    draw_box(min, ui_state.button_size, out, target_color);

    ui_state.__last_element_size = ui_state.button_size;

    return pressed;
}

void toggle(v2_i min, output_buffers& out, ui_state& ui_state, bool& state){
    const auto hovering = intersect_box(min, ui_state.button_size, ui_state.mouse_x, ui_state.mouse_y);
    const auto pressed = hovering && ui_state.mouse_down && ui_state.mouse_down_this_frame;
    
    //toggle off mouse
    if(pressed){
        ui_state.mouse_down = false;
        state = !state;
    }

    const auto col = hovering ? ui_state.toggle_hover_color :
                         state ? ui_state.toggle_active_color
                         : ui_state.toggle_inactive_color;

    draw_box(min, ui_state.button_size, out, col);

    ui_state.__last_element_size = ui_state.button_size;
}

static void blit_letter(v2_i btm_left, unsigned char letter, ui_state& ui_state, output_buffers& out, const rgba & color)
{
    const auto cell_width = 16;
    const auto cell_height = 16;
    const auto chars_per_row = 16;
    const auto start_char = 32;

    const auto val = static_cast<int>(letter) - start_char;

    assert(val < 128);
    assert(val > 0);

    const auto y_min  = 256 - (val / chars_per_row + 1) * cell_height;
    const auto y_max = y_min + cell_height;

    const auto x_min = (val % chars_per_row) * cell_width;
    const auto x_max = x_min + cell_width;

    const auto initial_x = btm_left.x;

    for(auto y = y_min; y < y_max; y++){
        btm_left.x = initial_x;

        for(auto x = x_min; x < x_max; x++){
            const auto sample_color = get_pixel(ui_state.letter_sampler, x, y);

            if(sample_color.a > 0){
                set_pixel(out.frame_buffer, color, btm_left.x, btm_left.y);
            }

            btm_left.x++;
        }
        btm_left.y++;
    }
}

void blit_string(v2_i min, const char * string, ui_state& ui_state, output_buffers& out, const rgba col)
{
    auto walk = min;
    
    const auto stride = 18 / 2;

    while(*string != '\0' && walk.x + stride < out.frame_buffer.width){
        //don't split space
        if (*string != ' '){
            blit_letter(walk, *string, ui_state, out, col);
        } 
        
        string++;

        walk.x += stride;
    }

    ui_state.__last_element_size = walk - min;
}

void float_selector(v2_i min, output_buffers& out, ui_state& ui_state, float& state, const float increment){
    auto walk = min;

    if (button(walk, out, ui_state)) {
        state -= increment;
    }

    increment_col(walk, ui_state);

    char buf[1024];
    FORMAT_PRINT(buf, "%.2f", 1024, state);
    blit_string(walk, buf, ui_state, out, ui_state.text_col);

    increment_col(walk, ui_state);

    if (button(walk, out, ui_state)) {
        state += increment;
    }

    increment_col(walk, ui_state);

    ui_state.__last_element_size = walk - min;
}

void int_selector(v2_i min, output_buffers& out, ui_state& ui_state, int& state, const int increment)
{
    auto walk = min;
    
    if (button(walk, out, ui_state)) {
        state -= increment;
    }

    increment_col(walk, ui_state);
   
    char buf[1024];
    FORMAT_PRINT(buf, "%d", 1024, state);
    blit_string(walk, buf, ui_state, out, ui_state.text_col);

    increment_col(walk, ui_state);

    if (button(walk, out, ui_state)) {
        state += increment;
    }

    increment_col(walk, ui_state);

    ui_state.__last_element_size = walk - min;
}