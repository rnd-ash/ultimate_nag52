//
// Created by ashcon on 2/21/21.
//

#include "virtual_kombi.h"
#include <SDL2/SDL_image.h>


#define WIN_WIDTH 1366
#define WIN_HEIGHT 768

void virtual_kombi::start(CAN_SIMULATOR* simulator) {
    this->sim = simulator;
    SDL_Init(SDL_INIT_VIDEO);
    this->window = SDL_CreateWindow(
        "Virtual Kombi (W203.007)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIN_WIDTH,
        WIN_HEIGHT,
        0
    );
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    this->bg_left = this->load_texture((char*)"v_kombi/img/bg_left.png", (char*)"BG_LEFT");
    this->bg_left.loc = SDL_Rect { // x y w h
            100,
            100,
            this->bg_left.w/3,
            this->bg_left.h/3
    };
    this->bg_right = this->load_texture((char*)"v_kombi/img/bg_right.png", (char*)"BG_RIGHT");
    // Set position
    this->bg_right.loc = SDL_Rect { // x y w h
        800,
        100-4, // IMPORTANT (-4) - Images don't line up perfectly!
        this->bg_right.w/3,
        this->bg_right.h/3
    };

    this->esp_light = this->load_texture((char*)"v_kombi/img/esp.png", (char*)"ESP_WARNING");
    this->esp_light.loc = SDL_Rect { // x y w h
            365,
            250,
            this->esp_light.w/3,
            this->esp_light.h/3
    };

    this->brake_light = this->load_texture((char*)"v_kombi/img/handbrake.png", (char*)"HANDBRAKE");
    this->brake_light.loc = SDL_Rect { // x y w h
            415,
            380,
            this->esp_light.w/3,
            (int)(this->esp_light.h/3.2)
    };

    this->abs_light = this->load_texture((char*)"v_kombi/img/abs.png", (char*)"ABS");
    this->abs_light.loc = SDL_Rect { // x y w h
            925,
            250,
            (int)(this->abs_light.w/2.9),
            (int)(this->abs_light.h/2.9)
    };

    this->srs_light = this->load_texture((char*)"v_kombi/img/srs.png", (char*)"ABS");
    this->srs_light.loc = SDL_Rect { // x y w h
            1025,
            255,
            (int)(this->srs_light.w/2.9),
            (int)(this->srs_light.h/2.9)
    };

    this->beam_light = this->load_texture((char*)"v_kombi/img/beam.png", (char*)"ABS");
    this->beam_light.loc = SDL_Rect { // x y w h
            975,
            375,
            (int)(this->beam_light.w/2.9),
            (int)(this->beam_light.h/2.9)
    };


    // Needles
    kombiPart img_needle_temp = this->load_texture((char*)"v_kombi/img/needle_small.png", (char*)"NEEDLE_TEMP");
    img_needle_temp.loc = SDL_Rect { // x y w h
            214,
            406,
            (int)(img_needle_temp.w/2.5),
                    (int)(img_needle_temp.h/2.5)
    };
    this->engine_temp = Needle {
        img_needle_temp,
        SDL_Point {
                (int)(48/2.5),
                        (int)(70/2.5)
        },
        90.0,
        40, // *C
        180.0,
        130, // *C
        90.0, // Same as minimum angle,
        90.0
    };

    // Needle for speed (MPH)
    kombiPart img_needle_spd = this->load_texture((char*)"v_kombi/img/needle_large.png", (char*)"NEEDLE_SPD");
    img_needle_spd.loc = SDL_Rect { // x y w h
            405,
            260,
            (int)(img_needle_spd.w/2.75),
            (int)(img_needle_spd.h/2.75)
    };
    this->speed = Needle {
            img_needle_spd,
            SDL_Point {
                    (int)(100.5/2.75),
                    (int)(150/2.75)
            },
            45.0,
            0, // MPH
            315.0,
            160, // MPH
            30.0, // Same as minimum angle,
            30.0
    };

    // Needle for engine RPM
    kombiPart img_needle_rpm = this->load_texture((char*)"v_kombi/img/needle_large.png", (char*)"NEEDLE_RPM");
    img_needle_rpm.loc = SDL_Rect { // x y w h
            965,
            260,
            (int)(img_needle_rpm.w/2.75),
            (int)(img_needle_rpm.h/2.75)
    };
    this->tachometer = Needle {
            img_needle_rpm,
            SDL_Point {
                    (int)(100.5/2.75),
                    (int)(150/2.75)
            },
            45.0,
            0, // RPM
            315.0,
            5000, // RPM
            30.0, // Same as minimum angle,
            30.0
    };

    kombiPart img_needle_fuel = this->load_texture((char*)"v_kombi/img/needle_small.png", (char*)"NEEDLE_TEMP");
    img_needle_fuel.loc = SDL_Rect { // x y w h
            1190,
            400,
            (int)(img_needle_fuel.w/2.5),
            (int)(img_needle_fuel.h/2.5)
    };
    this->fuel = Needle {
            img_needle_fuel,
            SDL_Point {
                    (int)(48/2.5),
                    (int)(70/2.5)
            },
            270.0,
            0, // %
            180.0,
            100, // %
            270.0,
            270.0
    };


    // Set default needle values!
    engine_temp.set_value(-40);
    speed.set_value(0);
    // Set position
    this->draw_ic();
}

void virtual_kombi::draw_kombi_part(kombiPart* p) {
    if (p->tex != nullptr && p->is_active) {
        SDL_RenderCopy(this->renderer, p->tex, nullptr, &p->loc);
    }
}

bool quit = false;
void virtual_kombi::draw_ic() {
   while (!quit) {
        this->update();
        //SDL_SetRenderDrawColor(this->renderer, 0, 128, 128, 1);
        SDL_RenderClear(this->renderer);
        // Draw background of IC
        draw_kombi_part(&this->bg_left);
        draw_kombi_part(&this->bg_right);

        // Draw warning lights
        draw_kombi_part(&this->esp_light);
        draw_kombi_part(&this->abs_light);
        draw_kombi_part(&this->brake_light);
        draw_kombi_part(&this->srs_light);
        draw_kombi_part(&this->beam_light);

        // Needles get drawn last (As they go over other elements in the img)
        draw_kombi_needle(&this->engine_temp);
        draw_kombi_needle(&this->speed);
        draw_kombi_needle(&this->tachometer);
        draw_kombi_needle(&this->fuel);

        // Present render
        SDL_RenderPresent(this->renderer);
    }
}

kombiPart virtual_kombi::load_texture(char *path, char *name) {
    printf("Loading texture %s from %s\n", name, path);
    SDL_Texture* tex = IMG_LoadTexture(this->renderer, path);
    int w, h;
    if (tex == nullptr) {
        printf("ERROR: Could not load texture %s from %s\n", name, path);
        return kombiPart {nullptr, SDL_Rect(), 0, 0, false};
    } else {
        SDL_QueryTexture(tex, NULL, NULL, &w, &h);
        printf("Load texture OK! Width: %d, height: %d\n", w, h);

    }
    SDL_Rect coords;
    kombiPart p {
        tex,
        coords,
        w,
        h,
        true
    };
    return p;
}

void virtual_kombi::draw_kombi_needle(Needle *needle) {
    needle->update_animation();
    SDL_RenderCopyEx(this->renderer, needle->img.tex, nullptr, &needle->img.loc, needle->curr_rotation_deg, &needle->rotation, SDL_FLIP_NONE);
}


SDL_Event e;
bool test_press = false;
void virtual_kombi::update() {

    // Kombi uses front wheel RPM (DVL + DVR) to calculate speed of vehicle)
    double avg_rpm = (bs200.get_DVL()/2.0 + bs200.get_DVR()/2.0) / 2.0;

    // https://www.tyresizecalculator.com/tyre-wheel-calculators/tire-size-calculator-tire-dimensions
    // My W203 has 245/40 R17 wheels

#define WHEEL_CIRCUMFERENCE_M 2.000

    double m_per_s = WHEEL_CIRCUMFERENCE_M * (double)avg_rpm / 60.0; // <- Revolution per sec
    int spd_mph = m_per_s * 2.23694;

    this->speed.set_value(spd_mph); // MPH
    this->tachometer.set_value(ms308.get_NMOT());
    this->engine_temp.set_value(ms608.get_T_MOT() - 40);
    this->fuel.set_value(100); // TODO fuel reading - Assume full tank

    // Handle key presses in window
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT) {
        quit = true;
    } else if (e.type == SDL_KEYDOWN) {
        ms308.set_NMOT(ms308.get_NMOT()+20);
        test_press = true;
    } else if (e.type == SDL_KEYUP) {
        test_press = false;
    }

    if (ms308.get_NMOT() > 700 && !test_press) {
        ms308.set_NMOT(ms308.get_NMOT()-1);
    }

}
