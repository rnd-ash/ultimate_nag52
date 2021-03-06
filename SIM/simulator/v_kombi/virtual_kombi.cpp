//
// Created by ashcon on 2/21/21.
//

#include "virtual_kombi.h"
#include <SDL2/SDL_image.h>


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

    // Overlay
    this->overlay = new diag_overlay();

    this->bg_left = this->load_texture((char*)"simulator/v_kombi/img/bg_left.png", (char*)"BG_LEFT");
    this->bg_left.loc = SDL_Rect { // x y w h
            100,
            100,
            this->bg_left.w/3,
            this->bg_left.h/3
    };
    this->bg_right = this->load_texture((char*)"simulator/v_kombi/img/bg_right.png", (char*)"BG_RIGHT");
    // Set position
    this->bg_right.loc = SDL_Rect { // x y w h
        800,
        100-4, // IMPORTANT (-4) - Images don't line up perfectly!
        this->bg_right.w/3,
        this->bg_right.h/3
    };

    this->esp_light = this->load_texture((char*)"simulator/v_kombi/img/esp.png", (char*)"ESP_WARNING");
    this->esp_light.loc = SDL_Rect { // x y w h
            365,
            250,
            this->esp_light.w/3,
            this->esp_light.h/3
    };

    this->brake_light = this->load_texture((char*)"simulator/v_kombi/img/handbrake.png", (char*)"HANDBRAKE");
    this->brake_light.loc = SDL_Rect { // x y w h
            415,
            380,
            this->esp_light.w/3,
            (int)(this->esp_light.h/3.2)
    };

    this->abs_light = this->load_texture((char*)"simulator/v_kombi/img/abs.png", (char*)"ABS");
    this->abs_light.loc = SDL_Rect { // x y w h
            925,
            250,
            (int)(this->abs_light.w/2.9),
            (int)(this->abs_light.h/2.9)
    };

    this->srs_light = this->load_texture((char*)"simulator/v_kombi/img/srs.png", (char*)"ABS");
    this->srs_light.loc = SDL_Rect { // x y w h
            1025,
            255,
            (int)(this->srs_light.w/2.9),
            (int)(this->srs_light.h/2.9)
    };

    this->beam_light = this->load_texture((char*)"simulator/v_kombi/img/beam.png", (char*)"ABS");
    this->beam_light.loc = SDL_Rect { // x y w h
            975,
            375,
            (int)(this->beam_light.w/2.9),
            (int)(this->beam_light.h/2.9)
    };


    // Needles
    kombiPart img_needle_temp = this->load_texture((char*)"simulator/v_kombi/img/needle_small.png", (char*)"NEEDLE_TEMP");
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
    kombiPart img_needle_spd = this->load_texture((char*)"simulator/v_kombi/img/needle_large.png", (char*)"NEEDLE_SPD");
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
    kombiPart img_needle_rpm = this->load_texture((char*)"simulator/v_kombi/img/needle_large.png", (char*)"NEEDLE_RPM");
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

    kombiPart img_needle_fuel = this->load_texture((char*)"simulator/v_kombi/img/needle_small.png", (char*)"NEEDLE_TEMP");
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
        SDL_SetRenderDrawColor(this->renderer, 0, 10, 10, 1);
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

        // Draw diag overlay
        this->overlay->draw_overlay(this->renderer, this->sim);

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
            case SDLK_p:
                this->sim->get_nag52()->request_upshift_press();
                break;
            case SDLK_o:
                this->sim->get_nag52()->request_downshift_press();
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
            case SDLK_p:
                this->sim->get_nag52()->request_upshift_release();
                break;
            case SDLK_o:
                this->sim->get_nag52()->request_downshift_release();
                break;
            default:
                break;
        }
    }
}

int counter = 0;
void virtual_kombi::update_loop() {

    while(!quit) {
        // https://www.tyresizecalculator.com/tyre-wheel-calculators/tire-size-calculator-tire-dimensions
        // My W203 has 245/40 R17 wheels
        this->animate_needles(); // Update IC needles etc...
        // Kombi uses front wheel RPM (DVL + DVR) to calculate speed of vehicle)
        double avg_rpm = (bs200.get_DVL() / 2.0 + bs200.get_DVR() / 2.0) / 2.0;

#define WHEEL_CIRCUMFERENCE_M 2.000 // In Meters

        double m_per_s = WHEEL_CIRCUMFERENCE_M * (double) avg_rpm / 60.0; // <- Revolution per sec
        int spd_mph = m_per_s * 2.23694;
        int spd_kmh = m_per_s * 3.6;

        this->speed.set_value(spd_mph); // MPH

        this->tachometer.set_value(ms308.get_NMOT());
        this->engine_temp.set_value(ms608.get_T_MOT() - 40);
        this->fuel.set_value(50); // TODO fuel reading - Assume half tank

        // Clear IC
        counter--;
        if (counter <= 0) {
            counter = 50;
            this->lcd->clear_screen();
            this->lcd->draw_text_large("IS-Test", 3, Justification::CENTER, false, false);
            this->lcd->draw_text_large("WSA NIO", 4, Justification::CENTER, false, false);
            this->lcd->draw_spd_kmh(spd_kmh);
            char prog = ' ';
            switch (gs418.get_FPC()) {
                case GS_FPC::W:
                case GS_FPC::W_MGN:
                case GS_FPC::W_MGW:
                    prog = 'W';
                    break;
                case GS_FPC::S:
                case GS_FPC::S_MCFB_WT:
                case GS_FPC::S_MGBB:
                case GS_FPC::S_MGGEA:
                case GS_FPC::S_MGN:
                case GS_FPC::S_MGSNN:
                case GS_FPC::S_MGW:
                case GS_FPC::S_MGZSN:
                    prog = 'S';
                    break;
                case GS_FPC::C:
                case GS_FPC::C_MCFB_WT:
                case GS_FPC::C_MGBB:
                case GS_FPC::C_MGGEA:
                case GS_FPC::C_MGN:
                case GS_FPC::C_MGSNN:
                case GS_FPC::C_MGW:
                case GS_FPC::C_MGZSN:
                    prog = 'C';
                    break;
                case GS_FPC::M:
                case GS_FPC::M_MGW:
                case GS_FPC::M_MGN:
                    prog = 'M';
                    break;
                case GS_FPC::UP:
                    prog = '^';
                    break;
                case GS_FPC::DOWN:
                    prog = 'v';
                    break;
                case GS_FPC::U:
                case GS_FPC::U_MGN:
                case GS_FPC::U_MGW:
                    prog = '_';
                    break;
                case GS_FPC::A:
                case GS_FPC::A_MCFB_WT:
                case GS_FPC::A_MGBB:
                case GS_FPC::A_MGGEA:
                case GS_FPC::A_MGN:
                case GS_FPC::A_MGSNN:
                case GS_FPC::A_MGW:
                case GS_FPC::A_MGZSN:
                    prog = 'A';
                    break;
                case GS_FPC::F:
                case GS_FPC::F_MGW:
                    prog = 'F';
                    break;
                default:
                    break;
            }

            switch (gs418.get_FSC()) { // Get text to be displays in the 'D' slot
                case GS_FSC::ONE: // D (Drive - range restrict 1st gear)
                    this->lcd->draw_gear_display(false, false, false, true, '1', prog);
                    break;
                case GS_FSC::TWO: // D (Drive - range restrict 2nd gear)
                    this->lcd->draw_gear_display(false, false, false, true, '2', prog);
                    break;
                case GS_FSC::THREE: // D (Drive - range restrict 3rd gear)
                    this->lcd->draw_gear_display(false, false, false, true, '3', prog);
                    break;
                case GS_FSC::FOUR: // D (Drive - range restrict 4th gear)
                    this->lcd->draw_gear_display(false, false, false, true, '4', prog);
                    break;
                case GS_FSC::FIVE: // D (Drive - range restrict 5th gear)
                    this->lcd->draw_gear_display(false, false, false, true, '5', prog);
                    break;
                case GS_FSC::SIX: // D (Drive - range restrict 6th gear)
                    this->lcd->draw_gear_display(false, false, false, true, '6', prog);
                    break;
                case GS_FSC::SEVEN: // D (Drive - range restrict 7th gear)
                    this->lcd->draw_gear_display(false, false, false, true, '7', prog);
                    break;
                case GS_FSC::A: // A (Drive - All wheel drive)
                    this->lcd->draw_gear_display(false, false, false, true, 'A', prog);
                    break;
                case GS_FSC::D: // D (Drive - Normal)
                    this->lcd->draw_gear_display(false, false, false, true, 'D', prog);
                    break;
                case GS_FSC::F: // F (Failed gearbox)
                    this->lcd->draw_gear_display(false, false, false, true, 'F', prog);
                    break;
                case GS_FSC::N: // N (Neutral)
                    this->lcd->draw_gear_display(false, false, true, false, 'D', prog);
                    break;
                case GS_FSC::P: // P (Park)
                    this->lcd->draw_gear_display(true, false, false, false, 'D', prog);
                    break;
                case GS_FSC::R: // R (Reverse)
                    this->lcd->draw_gear_display(false, true, false, false, 'D', prog);
                    break;
                case GS_FSC::BLANK: // Blank
                default: // SNV (Signal not available)
                    break;
            }
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
