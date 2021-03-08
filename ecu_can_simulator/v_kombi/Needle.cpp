//
// Created by ashcon on 2/21/21.
//

#include "virtual_kombi.h"

Needle::Needle(kombiPart img, SDL_Point rot_pos, int min_angle, int max_angle, int min_value, int max_value) {
    this->img = img;
    this->rotation = rot_pos;

    this->min_angle = min_angle;
    this->max_angle = max_angle;

    this->min_value_raw = min_value;
    this->max_value_raw = max_value;

    this->curr_shown_value = min_value;
    this->curr_angle = min_angle;
    this->target_angle = min_angle;

    this->d_rot = 0;
    this->d2_rot = 0;
    this->last_angle = min_angle;
    this->target_raw = min_value_raw;
    this->speedup = false;
    this->slowdown = false;
}

void Needle::set_value(int raw) {
    this->target_raw = raw;
    if (raw >= max_value_raw) {
        target_angle = max_angle;
        this->last_angle = curr_angle;
    } else if (raw <= min_value_raw) {
        target_angle = min_angle;
        this->last_angle = curr_angle;
    } else {
        // Get number of degrees per raw value
        double tmp = (max_angle-min_angle) / (max_value_raw-min_value_raw);
        target_angle = min_angle + ((raw-min_value_raw)*tmp);
        this->last_angle = curr_angle;
    }
}

void Needle::render(SDL_Renderer *r) {
    SDL_RenderCopyEx(r, this->img.tex, nullptr, &img.loc, this->curr_angle, &this->rotation, SDL_FLIP_NONE);
}


void Needle::update_motor() {
    // TODO we want a nice ease in-out function to apply to the motor needles
    // to minic the worm gears speeding up and slowing down that drive the needles
    // for now, Linear animation is good enough

    double min_delta = 1.0;
    if (abs(target_angle-curr_angle) < 1.0) {
        min_delta / 10;
    }

    if (this->target_angle > this->curr_angle) {
        this->curr_angle += std::fmin(min_delta, target_angle-curr_angle);
    } else if (this->target_angle < this->curr_angle) {
        this->curr_angle -= std::fmin(min_delta, curr_angle-target_angle);
    }
}

Needle::Needle() {

}
