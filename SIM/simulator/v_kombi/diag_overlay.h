//
// Created by ashcon on 3/13/21.
//

#ifndef NAG52_SIMULATOR_DIAG_OVERLAY_H
#define NAG52_SIMULATOR_DIAG_OVERLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../canbus/car_sim.h"

#include <deque>

#define WIN_WIDTH 1366
#define WIN_HEIGHT 768

struct Pwm_Reading {
    uint8_t tcc;
    uint8_t mpc;
    uint8_t spc;

    uint8_t y3;
    uint8_t y4;
    uint8_t y5;
};

#define MAX_READINGS 500

class diag_overlay {
public:
    diag_overlay();

    void draw_overlay(SDL_Renderer *r, CAR_SIMULATOR* sim);
private:
    void draw_can_data(SDL_Renderer *r, CAR_SIMULATOR* sim);
    void draw_pwm_chart(SDL_Renderer *r, CAR_SIMULATOR*sim);

    void draw_pressure_reading_line(SDL_Renderer *r, int x1, int x2, int press_val1, int press_val2);
    void draw_text(SDL_Renderer *r, SDL_Color colour, char* txt, int x, int y);
    TTF_Font* font;

    Pwm_Reading pressure_readings[MAX_READINGS] = {0x00};
};


#endif //NAG52_SIMULATOR_DIAG_OVERLAY_H
