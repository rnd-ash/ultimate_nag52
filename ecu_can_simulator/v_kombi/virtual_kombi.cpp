//
// Created by ashcon on 2/21/21.
//

#include "virtual_kombi.h"
#include <SDL2/SDL_image.h>


#define WIN_WIDTH 1366
#define WIN_HEIGHT 768

virtual_kombi::virtual_kombi(CAR_SIMULATOR *simulator) {
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
    this->lcd = new kombi_lcd(this->renderer);
    SDL_GL_SetSwapInterval(1);

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
    this->engine_temp = Needle(
        img_needle_temp,
        SDL_Point {
                (int)(48/2.5),
                        (int)(70/2.5)
        },
        90.0,
        180.0,
        40,
        130 // *C
    );

    // Needle for speed (MPH)
    kombiPart img_needle_spd = this->load_texture((char*)"v_kombi/img/needle_large.png", (char*)"NEEDLE_SPD");
    img_needle_spd.loc = SDL_Rect { // x y w h
            405,
            260,
            (int)(img_needle_spd.w/2.75),
            (int)(img_needle_spd.h/2.75)
    };
    this->speed = Needle(
            img_needle_spd,
            SDL_Point {
                    (int)(100.5/2.75),
                    (int)(150/2.75)
            },
            45.0,
            315.0,
            0,
            160 // MPH
    );

    // Needle for engine RPM
    kombiPart img_needle_rpm = this->load_texture((char*)"v_kombi/img/needle_large.png", (char*)"NEEDLE_RPM");
    img_needle_rpm.loc = SDL_Rect { // x y w h
            965,
            260,
            (int)(img_needle_rpm.w/2.75),
            (int)(img_needle_rpm.h/2.75)
    };
    this->tachometer = Needle(
            img_needle_rpm,
            SDL_Point {
                    (int)(100.5/2.75),
                    (int)(150/2.75)
            },
            45.0,
            315.0,
            0, // RPM
            5000 // RPM
    );

    kombiPart img_needle_fuel = this->load_texture((char*)"v_kombi/img/needle_small.png", (char*)"NEEDLE_TEMP");
    img_needle_fuel.loc = SDL_Rect { // x y w h
            1190,
            400,
            (int)(img_needle_fuel.w/2.5),
            (int)(img_needle_fuel.h/2.5)
    };
    this->fuel = Needle(
            img_needle_fuel,
            SDL_Point {
                    (int)(48/2.5),
                    (int)(70/2.5)
            },
            270.0,
            180.0,
            0, // %
            100 // %
    );


    // Set default needle values!
    engine_temp.set_value(-40);
    speed.set_value(0);

    // Launch simulation thread
    this->updater_thread = std::thread(&virtual_kombi::update_loop, this);
}

void virtual_kombi::draw_kombi_part(kombiPart* p) {
    if (p->tex != nullptr && p->is_active) {
        SDL_RenderCopy(this->renderer, p->tex, nullptr, &p->loc);
    }
}

SDL_Rect lcd_rect{ // 2.45
    632,
    120,
    (int)(IC_WIDTH_PX*2.45),
            (int)((IC_HEIGHT_BOT_PX+IC_HEIGHT_TOP_PX)*2.45),
};

bool quit = false;
void virtual_kombi::loop() {
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

        // Draw LCD
        SDL_RenderCopy(this->renderer, this->lcd->get_texture(), nullptr, &lcd_rect);

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
    needle->render(this->renderer);
}


SDL_Event e;
bool move_applied = false;
bool test_press = false;
void virtual_kombi::update() {

    // Handle key presses in window
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT) {
        quit = true;
    } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_e:
                bs200.set_DVL(bs200.get_DVL() + 10);
                bs200.set_DVR(bs200.get_DVL() + 10);
                break;
            case SDLK_q:
                bs200.set_DVL(bs200.get_DVL() - 10);
                bs200.set_DVR(bs200.get_DVR() - 10);
                break;
            case SDLK_w:
                sim->get_engine()->press_pedal();
                break;
            case SDLK_s:
                // TODO
                break;
            case SDLK_f:
                sim->get_ewm()->press_btn();
                break;
            case SDLK_i:
                if(!move_applied) { move_applied = true; sim->get_ewm()->apply_force(MOVE_DIR::UP); }
                break;
            case SDLK_k:
                if(!move_applied) { move_applied = true; sim->get_ewm()->apply_force(MOVE_DIR::DOWN); }
                break;
            case SDLK_j:
                if(!move_applied) { move_applied = true; sim->get_ewm()->apply_force(MOVE_DIR::LEFT); }
                break;
            case SDLK_l:
                if(!move_applied) { move_applied = true; sim->get_ewm()->apply_force(MOVE_DIR::RIGHT); }
                break;
            default:
                break;
        }
    } else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_w:
                sim->get_engine()->release_pedal();
                break;
            case SDLK_f:
                sim->get_ewm()->release_btn();
                break;
            case SDLK_i:
            case SDLK_k:
            case SDLK_j:
            case SDLK_l:
                move_applied = false;
                sim->get_ewm()->apply_force(MOVE_DIR::NONE);
                break;
            default:
                break;
        }
    }
}

void virtual_kombi::update_loop() {

    while(!quit) {
        // Clear IC
        this->lcd->clear_screen();
        //this->lcd->draw_text_small("Settings", 1, Justification::CENTER, false, false);
        //this->lcd->draw_text_small("To reset:", 2, Justification::CENTER, false, false);
        //this->lcd->draw_text_small("Press reset", 3, Justification::CENTER, false, false);
        //this->lcd->draw_text_small("button", 4, Justification::CENTER, false, false);
        //this->lcd->draw_text_small("for 3 seconds", 5, Justification::CENTER, false, false);

        this->lcd->draw_text_large("IS-Test", 3, Justification::CENTER, false, false);
        this->lcd->draw_text_large("WSA NIO", 4, Justification::CENTER, false, false);

        this->animate_needles(); // Update IC needles etc...
        // Kombi uses front wheel RPM (DVL + DVR) to calculate speed of vehicle)
        double avg_rpm = (bs200.get_DVL() / 2.0 + bs200.get_DVR() / 2.0) / 2.0;

        // https://www.tyresizecalculator.com/tyre-wheel-calculators/tire-size-calculator-tire-dimensions
        // My W203 has 245/40 R17 wheels

#define WHEEL_CIRCUMFERENCE_M 2.000 // In Meters

        double m_per_s = WHEEL_CIRCUMFERENCE_M * (double) avg_rpm / 60.0; // <- Revolution per sec
        int spd_mph = m_per_s * 2.23694;
        int spd_kmh = m_per_s * 3.6;

        this->speed.set_value(spd_mph); // MPH
        this->lcd->draw_spd_kmh(spd_kmh);
        this->tachometer.set_value(ms308.get_NMOT());
        this->engine_temp.set_value(ms608.get_T_MOT() - 40);
        this->fuel.set_value(50); // TODO fuel reading - Assume half tank

        char prog = ' ';
        switch(gs418.get_FPC()) {
            case DrivingProgram::W:
            case DrivingProgram::W_MGN:
            case DrivingProgram::W_MGW:
                prog = 'W';
                break;
            case DrivingProgram::S:
            case DrivingProgram::S_MCFB_WT:
            case DrivingProgram::S_MGBB:
            case DrivingProgram::S_MGGEA:
            case DrivingProgram::S_MGN:
            case DrivingProgram::S_MGSNN:
            case DrivingProgram::S_MGW:
            case DrivingProgram::S_MGZSN:
                prog = 'S';
                break;
            case DrivingProgram::C:
            case DrivingProgram::C_MCFB_WT:
            case DrivingProgram::C_MGBB:
            case DrivingProgram::C_MGGEA:
            case DrivingProgram::C_MGN:
            case DrivingProgram::C_MGSNN:
            case DrivingProgram::C_MGW:
            case DrivingProgram::C_MGZSN:
                prog = 'C';
                break;
            case DrivingProgram::M:
            case DrivingProgram::M_MGW:
            case DrivingProgram::M_MGN:
                prog = 'M';
                break;
            case DrivingProgram::UP:
                prog = '^';
                break;
            case DrivingProgram::DOWN:
                prog = 'v';
                break;
            case DrivingProgram::U:
            case DrivingProgram::U_MGN:
            case DrivingProgram::U_MGW:
                prog = '_';
                break;
            case DrivingProgram::A:
            case DrivingProgram::A_MCFB_WT:
            case DrivingProgram::A_MGBB:
            case DrivingProgram::A_MGGEA:
            case DrivingProgram::A_MGN:
            case DrivingProgram::A_MGSNN:
            case DrivingProgram::A_MGW:
            case DrivingProgram::A_MGZSN:
                prog = 'A';
                break;
            case DrivingProgram::F:
            case DrivingProgram::F_MGW:
                prog = 'F';
                break;
            default:
                break;
        }

        switch (gs418.get_FSC()) { // Get text to be displays in the 'D' slot
            case 49: // D (Drive - range restrict 1st gear)
                this->lcd->draw_gear_display(false, false, false, true, '1', prog);
                break;
            case 50: // D (Drive - range restrict 2nd gear)
                this->lcd->draw_gear_display(false, false, false, true, '2', prog);
                break;
            case 51: // D (Drive - range restrict 3rd gear)
                this->lcd->draw_gear_display(false, false, false, true, '3', prog);
                break;
            case 52: // D (Drive - range restrict 4th gear)
                this->lcd->draw_gear_display(false, false, false, true, '4', prog);
                break;
            case 53: // D (Drive - range restrict 5th gear)
                this->lcd->draw_gear_display(false, false, false, true, '5', prog);
                break;
            case 54: // D (Drive - range restrict 6th gear)
                this->lcd->draw_gear_display(false, false, false, true, '6', prog);
                break;
            case 55: // D (Drive - range restrict 7th gear)
                this->lcd->draw_gear_display(false, false, false, true, '7', prog);
                break;
            case 65: // A (Drive - All wheel drive)
                this->lcd->draw_gear_display(false, false, false, true, 'A', prog);
                break;
            case 68: // D (Drive - Normal)
                this->lcd->draw_gear_display(false, false, false, true, 'D', prog);
                break;
            case 70: // F (Failed gearbox)
                this->lcd->draw_gear_display(false, false, false, true, 'F', prog);
                break;
            case 78: // N (Neutral)
                this->lcd->draw_gear_display(false, false, true, false, 'D', prog);
                break;
            case 80: // P (Park)
                this->lcd->draw_gear_display(true, false, false, false, 'D', prog);
                break;
            case 82: // R (Reverse)
                this->lcd->draw_gear_display(false, true, false, false, 'D', prog);
                break;
            case 32: // Blank
            default: // SNV (Signal not available)
                break;
        }
        this->abs_light.is_active = bs200.get_ABS_KL();
        this->esp_light.is_active = bs200.get_ESP_INFO_DL() | bs200.get_ESP_INFO_BL();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void virtual_kombi::animate_needles() {
    this->speed.update_motor();
    this->fuel.update_motor();
    this->tachometer.update_motor();
    this->engine_temp.update_motor();
}
