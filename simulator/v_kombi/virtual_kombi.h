//
// Created by ashcon on 2/21/21.
//

#ifndef ECU_CAN_SIMULATOR_VIRTUAL_KOMBI_H
#define ECU_CAN_SIMULATOR_VIRTUAL_KOMBI_H

#include <SDL2/SDL.h>
#include "../canbus/car_sim.h"
#include "kombi_lcd.h"
#include "diag_overlay.h"

struct kombiPart {
    SDL_Texture *tex;
    SDL_Rect loc;
    int w;
    int h;
    bool is_active;
};

// TODO - Note to self, should move needle animations to another thread :)
#define NEEDLE_ROT_SPEED 1 // 100 of these per second

class Needle {
public:
    Needle();
    Needle(kombiPart img, SDL_Point rot_pos, int min_angle, int max_angle, int min_value, int max_value);
    void set_value(int raw);
    void update_motor();
    void render(SDL_Renderer *r);
private:
    kombiPart img;
    SDL_Point rotation;
    double min_angle;
    double min_value_raw;
    double max_angle;
    double max_value_raw;
    double curr_angle;
    double target_angle;
    double curr_shown_value;
    double d_rot;
    double d2_rot;
    double last_angle;
    bool speedup;
    bool slowdown;
    double target_raw;
};

class virtual_kombi {
public:
    virtual_kombi(CAR_SIMULATOR *simulator);
    void update();

    void loop();

private:
    kombiPart load_texture(char* path, char* name);
    diag_overlay* overlay; // For diagnostic
    void draw_kombi_part(kombiPart* p);
    void draw_kombi_needle(Needle* needle);
    void update_loop();

    void animate_needles();

    std::thread updater_thread;
    CAR_SIMULATOR* sim;

    SDL_Window *window;
    SDL_Renderer *renderer;
    kombiPart bg_right;
    kombiPart bg_left;

    // Needles
    Needle engine_temp;
    Needle speed;
    Needle tachometer;
    Needle fuel;

    // Lights
    kombiPart abs_light;
    kombiPart brake_light;
    kombiPart esp_light;
    kombiPart srs_light;
    kombiPart beam_light;

    kombi_lcd* lcd = nullptr;


};

#endif //ECU_CAN_SIMULATOR_VIRTUAL_KOMBI_H
