//
// Created by ashcon on 2/21/21.
//

#ifndef ECU_CAN_SIMULATOR_KOMBI_LCD_H
#define ECU_CAN_SIMULATOR_KOMBI_LCD_H

#include <SDL2/SDL.h>
#include "lcd_symbols.h"

// IC width = 70px (38mm) 1.85px/mm
// IC height (Top portion) = 100 (54mm) 1.85px/mm
// IC height (Bottom portion) = 60 (33mm) 1.85px/mm

#define IC_WIDTH_PX 72
#define IC_WIDTH_MM 38

#define IC_HEIGHT_TOP_PX 100
#define IC_HEIGHT_TOP_MM 54
#define IC_HEIGHT_BOT_PX 60
#define IC_HEIGHT_BOT_MM 33

#define PIX_ON_WHITE  0xFF61cffc ;
#define PIX_OFF_WHITE 0xFF031e42;
#define PIX_ON_RED    0xFFC00000;
#define PIX_OFF_RED   0xFF400000;
#define NUM_PIXELS IC_WIDTH_PX * (IC_HEIGHT_BOT_PX+IC_HEIGHT_TOP_PX)


enum Justification {
    CENTER,
    LEFT,
    RIGHT
};

class kombi_lcd {

public:
    kombi_lcd(SDL_Renderer* r);
    SDL_Texture* get_texture();
    void set_red(bool is_red);
    void draw_spd_kmh(int spd);
    void clear_screen();
    // Draws test in the large LCD of the IC
    void draw_text_small(char* txt, int row, Justification j, bool is_bold, bool is_highlighted);
    void draw_text_large(char* txt, int row, Justification j, bool is_bold, bool is_highlighted);
    void toggle_cc_display(bool state);
private:
    bool show_cc = false;
    int draw_char_large(char x, int x_pos, int y_pos, bool is_bold, bool is_highlighted);
    int draw_char_small(char x, int x_pos, int y_pos, bool is_bold, bool is_highlighted);
    int draw_ascii(char c, ascii_table* t, int x_left, int y_bottom, bool is_highlighted);

    int get_ascii_size(char* txt, ascii_table* t);
    bool use_warning_colour = false;
    bool px_states[NUM_PIXELS] = {false};
    uint32_t pixel_buffer[NUM_PIXELS] = {0x00}; // All off by default
    SDL_Texture* texture;

    ascii_table std;
    ascii_table std_bold;
    ascii_table large;
    ascii_table large_bold;

    void draw_time();
};


#endif //ECU_CAN_SIMULATOR_KOMBI_LCD_H
