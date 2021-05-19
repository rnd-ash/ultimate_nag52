//
// Created by ashcon on 2/22/21.
//

#ifndef ECU_CAN_SIMULATOR_LCD_SYMBOLS_H
#define ECU_CAN_SIMULATOR_LCD_SYMBOLS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct lcd_char {
    int h;
    int w;
    int draw_width;
    bool* buf;
};

class ascii_table {
public:
    ascii_table(SDL_Renderer* r, char* path, int char_width, int char_height);
    ascii_table();
    ~ascii_table();
    lcd_char* get_buffer(char c);
private:
    lcd_char* state_buffer;
    int char_width = 0;
    int char_height = 0;
    uint32_t* bytes = nullptr;
};

#endif //ECU_CAN_SIMULATOR_LCD_SYMBOLS_H
