//
// Created by ashcon on 3/13/21.
//

#include "diag_overlay.h"

const SDL_Color textColour = {255, 255, 255, 255};

const SDL_Color spc_colour = {87, 218, 62, 255};
const SDL_Color mpc_colour = {119, 49, 188, 255};
const SDL_Color tcc_colour = {131, 120, 27, 255};

const SDL_Color y3_colour = {255, 0, 255, 255};
const SDL_Color y4_colour = {0, 255, 0, 255};
const SDL_Color y5_colour = {255, 0, 0, 255};

void diag_overlay::draw_overlay(SDL_Renderer *r, CAR_SIMULATOR* sim) {
    this->draw_can_data(r, sim);
    this->draw_pwm_chart(r, sim);
}

void diag_overlay::draw_can_data(SDL_Renderer *r, CAR_SIMULATOR *sim) {
    std::string s = "CAN DATA:\n\n";
    for (auto &i : sim->last_frames) {
        s += fmt_frame(&i.second) + "\n";
    }

    SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(this->font, s.c_str(), textColour, 500);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(r, textSurface);

    SDL_Rect textRect;
    textRect.x = 0;
    textRect.y = 0;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(r, textTexture, nullptr, &textRect);

    // Don't forget to tidy up :)
    SDL_FreeSurface(textSurface);
    SDL_UnlockTexture(textTexture);
    SDL_DestroyTexture(textTexture);
}

diag_overlay::diag_overlay() {
    TTF_Init();
    this->font = TTF_OpenFont("text_font.ttf", 14);
}

void diag_overlay::draw_text(SDL_Renderer *r, SDL_Color colour, char* txt, int x, int y) {
    SDL_Surface *textSurface = TTF_RenderText_Blended(this->font, txt, colour);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(r, textSurface);
    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderCopy(r, textTexture, nullptr, &textRect);

    // Don't forget to tidy up :)
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    SDL_UnlockTexture(textTexture);
    SDL_DestroyTexture(textTexture);
}

void diag_overlay::draw_pwm_chart(SDL_Renderer *r, CAR_SIMULATOR *sim) {
    // Get next pressure readings
    nag52* n = sim->get_nag52();

    Pwm_Reading reading {
        n->get_tcc_duty(),
        n->get_mpc_duty(),
        n->get_spc_duty(),
        n->get_y3_duty(),
        n->get_y4_duty(),
        n->get_y5_duty()
    };
    // Append to list
    memcpy(&pressure_readings[1], &pressure_readings[0], (MAX_READINGS-1) * sizeof(struct Pwm_Reading));
    this->pressure_readings[0] = reading;

    for (int i = 0; i < MAX_READINGS-2; i++) { // i is position!
        Pwm_Reading* prev = &this->pressure_readings[i];
        Pwm_Reading* curr = &this->pressure_readings[i+1];
        SDL_SetRenderDrawColor(r, spc_colour.r, spc_colour.g, spc_colour.b, 255);
        this->draw_pressure_reading_line(r, i, i+1, prev->spc, curr->spc);

        SDL_SetRenderDrawColor(r, mpc_colour.r, mpc_colour.g, mpc_colour.b, 255);
        this->draw_pressure_reading_line(r, i, i+1, prev->mpc, curr->mpc);

        SDL_SetRenderDrawColor(r, tcc_colour.r, tcc_colour.g, tcc_colour.b, 255);
        this->draw_pressure_reading_line(r, i, i+1, prev->tcc, curr->tcc);

        SDL_SetRenderDrawColor(r, y3_colour.r, y3_colour.g, y3_colour.b, 255);
        this->draw_pressure_reading_line(r, i, i+1, prev->y3, curr->y3);

        SDL_SetRenderDrawColor(r, y4_colour.r, y4_colour.g, y4_colour.b, 255);
        this->draw_pressure_reading_line(r, i, i+1, prev->y4, curr->y4);

        SDL_SetRenderDrawColor(r, y5_colour.r, y5_colour.g, y5_colour.b, 255);
        this->draw_pressure_reading_line(r, i, i+1, prev->y5, curr->y5);
    }

    // Draw axis
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderDrawLine(r, WIN_WIDTH-1000, WIN_HEIGHT-1, WIN_WIDTH, WIN_HEIGHT-1); // X axis
    SDL_RenderDrawLine(r, WIN_WIDTH-1000, WIN_HEIGHT, WIN_WIDTH-1000, WIN_HEIGHT-255); // Y axis
    // Draw header text
    draw_text(r, textColour, (char*)"722.6 Solenoid duty cycle history", WIN_WIDTH- 700, WIN_HEIGHT-255);

    draw_text(r, textColour, (char*)"255 (On)", WIN_WIDTH-1070, WIN_HEIGHT-255);
    draw_text(r, textColour, (char*)"0 (Off)", WIN_WIDTH-1050, WIN_HEIGHT-20);

    // Draw legend (colours)
    draw_text(r, spc_colour, (char*)"Shift pressure", WIN_WIDTH-1200, WIN_HEIGHT-100);
    draw_text(r, mpc_colour, (char*)"Modulating pressure", WIN_WIDTH-1200, WIN_HEIGHT-120);
    draw_text(r, tcc_colour, (char*)"Torque converter", WIN_WIDTH-1200, WIN_HEIGHT-140);
    draw_text(r, y3_colour, (char*)"Y3 (1-2 & 4-5 shift)", WIN_WIDTH-1200, WIN_HEIGHT-160);
    draw_text(r, y4_colour, (char*)"Y4 (2-3 shift)", WIN_WIDTH-1200, WIN_HEIGHT-180);
    draw_text(r, y5_colour, (char*)"Y5 (3-4 shift)", WIN_WIDTH-1200, WIN_HEIGHT-200);
}

void diag_overlay::draw_pressure_reading_line(SDL_Renderer *r, int x1, int x2, int press_val1, int press_val2) {
    int real_x = WIN_WIDTH - (WIN_WIDTH-x1*(1000/MAX_READINGS));
    int real_x1 = WIN_WIDTH - (WIN_WIDTH-x2*(1000/MAX_READINGS));

    SDL_RenderDrawLine(r, WIN_WIDTH-real_x, WIN_HEIGHT-press_val1, WIN_WIDTH-real_x1, WIN_HEIGHT-press_val2);
}
