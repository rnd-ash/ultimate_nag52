//
// Created by ashcon on 2/21/21.
//

#ifndef ECU_CAN_SIMULATOR_VIRTUAL_KOMBI_H
#define ECU_CAN_SIMULATOR_VIRTUAL_KOMBI_H

#include <SDL2/SDL.h>
#include "../canbus/can_sim.h"

struct kombiPart {
    SDL_Texture *tex;
    SDL_Rect loc;
    int w;
    int h;
    bool is_active;
};

// TODO - Note to self, should move needle animations to another thread :)
#define NEEDLE_ROT_SPEED 0.03 // per frame

struct Needle {
public:
    kombiPart img;
    SDL_Point rotation;
    double min_angle;
    int min_value_raw;
    double max_angle;
    int max_value_raw;
    double curr_rotation_deg;
    double target_rotation_deg;
    void set_value(int raw) {
        if (raw >= max_value_raw) {
            target_rotation_deg = max_angle;
        } else if (raw <= min_value_raw) {
            target_rotation_deg = min_angle;
        } else {
            // Get number of degrees per raw value
            double tmp = (max_angle-min_angle) / ((double)max_value_raw-(double)min_value_raw);
            target_rotation_deg = min_angle + ((raw-min_value_raw)*tmp);
        }
    }
    void update_animation() {
        double delta = abs(target_rotation_deg- curr_rotation_deg);
        if (curr_rotation_deg <= target_rotation_deg) {
            curr_rotation_deg += std::fmin(NEEDLE_ROT_SPEED, delta);
        } else if (curr_rotation_deg >= target_rotation_deg) {
            curr_rotation_deg -= std::fmin(NEEDLE_ROT_SPEED, delta);
        }



    }
};

class virtual_kombi {
public:
    void start(CAN_SIMULATOR *simulator);
    void update();
private:
    kombiPart load_texture(char* path, char* name);
    [[noreturn]] void draw_ic();
    void draw_kombi_part(kombiPart* p);
    void draw_kombi_needle(Needle* needle);
    CAN_SIMULATOR* sim;

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
};

#endif //ECU_CAN_SIMULATOR_VIRTUAL_KOMBI_H
