#ifndef UI_H
#define UI_H

#include "render.h"

struct ui_state
{
    image letter_sampler;

    rgba button_color{};
    rgba button_hover_color{};
    rgba button_press_color{};

    rgba toggle_active_color{};
    rgba toggle_hover_color{};
    rgba toggle_inactive_color{};

    rgba text_col{};
	
    v2_i button_size{ 15, 15 };
    v2_i col_padding{ 10, 0  };
    v2_i row_padding{ 0,  20 };
    v2_i screen_margin{ 15,  15 };

    bool mouse_down{};
    bool mouse_down_this_frame{};
    bool mouse_in_window{};
    int mouse_x{};
    int mouse_y{};

	//draw mode
    bool draw_rows_up = true;
    bool draw_cols_right = true;
    int row_start_x {};
	
    //the size of the last rendered element, used for row increment calculations
    v2_i __last_element_size{};
};

inline void increment_row(v2_i& offset, const ui_state& state);
inline void increment_col(v2_i& offset, const ui_state& state);

void labeled_toggle(v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output, const char* label, bool& state);
void labeled_string(v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output, char* label, const char* string);
void left_right_selector(v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output, const char* label, bool& left_clicked, bool& right_clicked);
bool labeled_button(v2_i& ui_draw_position, ui_state& ui_state, output_buffers& output, const char* label);

bool button(const v2_i& min, output_buffers& out, ui_state& ui_state);
void toggle(v2_i min, output_buffers& out, ui_state& ui_state, bool& state);
void blit_string(v2_i min, const char * string, ui_state& ui_state, output_buffers& out, const rgba col);
void float_selector(v2_i min, output_buffers& out, ui_state& ui_state, float & state, const float increment);
void int_selector(v2_i min, output_buffers& out, ui_state& ui_state, int& state, const int increment);

#endif