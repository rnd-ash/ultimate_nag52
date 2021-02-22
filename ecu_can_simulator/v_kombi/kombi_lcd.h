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

#define IC_WIDTH_PX 70
#define IC_WIDTH_MM 38

#define IC_HEIGHT_TOP_PX 100
#define IC_HEIGHT_TOP_MM 54
#define IC_HEIGHT_BOT_PX 60
#define IC_HEIGHT_BOT_MM 33

#define PIX_ON_WHITE  0xFFFFFFFF;
#define PIX_OFF_WHITE 0xFF191919;
#define PIX_ON_RED    0xFFC00000;
#define PIX_OFF_RED   0xFF400000;
#define NUM_PIXELS IC_WIDTH_PX * (IC_HEIGHT_BOT_PX+IC_HEIGHT_TOP_PX)



class kombi_lcd {

public:
    kombi_lcd(SDL_Renderer* r);
    SDL_Texture* get_texture();
    void set_red(bool is_red);
    void draw_spd_kmh(int spd);
    void clear_screen();
private:
    template<int W, int H> void draw_pict(const ic_pict<W,H> &pict, int start_x, int bottom_y);
    int draw_char_large(char x, int x_pos, int y_pos);

    bool use_warning_colour = false;
    bool px_states[NUM_PIXELS] = {false};
    uint32_t pixel_buffer[NUM_PIXELS] = {0x00}; // All off by default
    SDL_Texture* texture;
};


#endif //ECU_CAN_SIMULATOR_KOMBI_LCD_H
