//
// Created by ashcon on 2/21/21.
//

#include <algorithm>
#include "kombi_lcd.h"

kombi_lcd::kombi_lcd(SDL_Renderer* r) {
    this->texture = SDL_CreateTexture(
            r,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STATIC,
            IC_WIDTH_PX,
            (IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX)
    );
    for (bool & i : this->px_states) {
        i = false;
    }
    this->use_warning_colour = false;
}

SDL_Texture *kombi_lcd::get_texture() {

    // Update texture
    for (int i = 0; i < IC_WIDTH_PX*IC_HEIGHT_TOP_PX; i++) {
        if (px_states[i]) {
            if (this->use_warning_colour) { this->pixel_buffer[i] = PIX_ON_RED } else { this->pixel_buffer[i] = PIX_ON_WHITE }
        } else {
            if (this->use_warning_colour) { this->pixel_buffer[i] = PIX_OFF_RED } else { this->pixel_buffer[i] = PIX_OFF_WHITE }
        }
    }
    // Bottom only uses black and white text, no Red
    for (int i = IC_WIDTH_PX*IC_HEIGHT_TOP_PX; i < NUM_PIXELS; i++) {
        if (px_states[i]) {
            pixel_buffer[i] = PIX_ON_WHITE
        } else {
            pixel_buffer[i] = PIX_OFF_WHITE
        }
    }

    SDL_UpdateTexture(this->texture, nullptr, this->pixel_buffer, IC_WIDTH_PX * sizeof(uint32_t));
    return this->texture;
}

void kombi_lcd::set_red(bool is_red) {
    this->use_warning_colour = is_red;
}

int kombi_lcd::draw_char_large(char x, int x_pos, int y_pos) {
    switch (x) {
        case '0':
            this->draw_pict(ZERO_LARGE, x_pos, y_pos);
            return ZERO_LARGE.w;
        case '1':
            this->draw_pict(ONE_LARGE, x_pos, y_pos);
            return ONE_LARGE.w;
        case '2':
            this->draw_pict(TWO_LARGE, x_pos, y_pos);
            return TWO_LARGE.w;
        case 'k':
            this->draw_pict(K_LOW_LARGE, x_pos, y_pos);
            return K_LOW_LARGE.w;
        case 'm':
            this->draw_pict(M_LOW_LARGE, x_pos, y_pos);
            return M_LOW_LARGE.w;
        case 'h':
            this->draw_pict(H_LOW_LARGE, x_pos, y_pos);
            return H_LOW_LARGE.w;
        case '/':
            this->draw_pict(SLASH_LARGE, x_pos, y_pos);
            return SLASH_LARGE.w;
        case ' ':
            return 2;
        default:
            return 0;
    }
}

void kombi_lcd::draw_spd_kmh(int spd) {
    char buf[10];
    int count = sprintf(buf, "%d km/h", spd);
    int x_pos = (IC_WIDTH_PX-10);
    for (int i = count-1; i >= 0; i--) {
        // Draw speed backwards
        x_pos -= (this->draw_char_large(buf[i], x_pos, IC_HEIGHT_TOP_PX + 20) + 1);
    }
}

template<int W, int H>
void kombi_lcd::draw_pict(const ic_pict<W, H> &pict, int start_x, int bottom_y) {
    for (int y = pict.h-1; y >= 0; y--) {
        // Draw from bottom up, so text is always on the same y level as its base!
        int y_pos = bottom_y - (pict.h - y); // position in LCD
        if (y >= 0 && y < IC_WIDTH_PX) {
            for (int x = 0; x < pict.w; x++) {
                int x_pos = start_x + x;// position in LCD
                if (x_pos > 0 && x_pos < IC_HEIGHT_TOP_PX+IC_HEIGHT_BOT_PX) {
                    bool state = false;
                    if (pict.buf[(pict.w*y)+x]) {
                        state = true;
                    }
                    this->px_states[(IC_WIDTH_PX*y_pos)+x_pos] = state;
                }
            }
        }
    }
}

void kombi_lcd::clear_screen() {
    for (bool & px_state : this->px_states) {
        px_state = false;
    }
}
