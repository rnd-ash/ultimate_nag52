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
    this->std = ascii_table(r, (char*)"v_kombi/img/lcd/ascii_small_std.png", 5, 7);
    this->std_bold = ascii_table(r, (char*)"v_kombi/img/lcd/ascii_big_std.png", 7, 10);
    this->large = ascii_table(r, (char*)"v_kombi/img/lcd/ascii_big_std.png", 7, 10);
    this->large_bold = ascii_table(r, (char*)"v_kombi/img/lcd/ascii_big_bold.png", 7, 10);
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

void kombi_lcd::draw_spd_kmh(int spd) {
    char buf[3];
    sprintf(buf, "%d", spd);

    int start = IC_WIDTH_PX/2 + 1;
    for (const char &x : "km/h") {
        start += this->draw_char_large(x, start, 10*12, false, false) + 1;
    }

    // Note to self - drawing speed is strange as each char occupies 7 pixels regardless of the text's actual width!
    start = IC_WIDTH_PX/2 -1 - 7*(strlen(buf)) - 2;
    for (const char &x : buf) {
        this->draw_char_large(x, start, 10*12, false, false);
        start += 7 + 1;
    }
    if (this->show_cc) {
        // TODO cruise control display bar

    } else {
        this->draw_time();
    }
}

void kombi_lcd::draw_time() {
    char buf[2];
    int h = 12;
    int m = 13;

    // draw colon in center
    this->draw_char_large(':', (IC_WIDTH_PX/2)-1, 11*12, false, false);
    int h_start = IC_WIDTH_PX/2+1;
    sprintf(buf, "%02d", h); // Tmp time
    for (const char &x : buf) {
        h_start += this->draw_char_large(x, h_start, 11*12, false, false) + 1;
    }
    sprintf(buf, "%02d", m); // Tmp time
    h_start = IC_WIDTH_PX/2-1 - get_ascii_size(buf, &this->large) -3;
    sprintf(buf, "%02d", h); // Tmp time
    for (const char &x : buf) {
        h_start += this->draw_char_large(x, h_start, 11*12, false, false) + 1;
    }
    //this->draw_text_large(buf, 11, Justification::CENTER, false, false);
}

void kombi_lcd::clear_screen() {
    for (bool & px_state : this->px_states) {
        px_state = false;
    }
}

int kombi_lcd::draw_char_small(char x, int x_pos, int y_pos, bool is_bold, bool is_highlighted) {
    if (is_bold) {
        return draw_ascii(x, &this->std_bold, x_pos, y_pos, is_highlighted);
    } else {
        return draw_ascii(x, &this->std, x_pos, y_pos, is_highlighted);
    }
}


int kombi_lcd::draw_char_large(char x, int x_pos, int y_pos, bool is_bold, bool is_highlighted) {
    if (is_bold) {
        return draw_ascii(x, &this->large_bold, x_pos, y_pos, is_highlighted);
    } else {
        return draw_ascii(x, &this->large, x_pos, y_pos, is_highlighted);
    }
}

int kombi_lcd::draw_ascii(char c, ascii_table *t, int x_left, int y_bottom, bool is_highlighted) {
    lcd_char* l = t->get_buffer(c);
    if (l == nullptr) {
        return 0;
    }

    // y_pos - bottom of the row
    // x_pos - left of the image
    for (int y = l->h-1; y >= 0; y--) {

        // Draw from bottom up, so text is always on the same y level as its base!
        int y_pos = y_bottom - (l->h - y); // position in LCD
        if (y_pos >= 0 && y_pos < IC_HEIGHT_TOP_PX+IC_HEIGHT_BOT_PX) {
            for (int x = 0; x < l->w; x++) {
                int x_pos = x_left + x;// position in LCD
                if (x_pos >= 0 && x_pos < IC_WIDTH_PX) {
                    bool state = l->buf[(l->w*y)+x];
                    if (is_highlighted) { // Highlight so invert the state
                        state =!state;
                    }
                    //printf("(%d, %d) - (%d %d) - %d\n", x_pos, y_pos, x, y, state);
                    if (state != this->px_states[(IC_WIDTH_PX * y_pos) + x_pos]) {
                        this->px_states[(IC_WIDTH_PX * y_pos) + x_pos] = state;
                    }
                }
            }
        }
    }
    //printf(" %c - %d\n", c, l->w);
    return l->draw_width;
}

void kombi_lcd::draw_text_small(char *txt, int row, Justification j, bool is_bold, bool is_highlighted) {
    // Small text is always 9 px
    int start_y = row * 12;
    int w = 1;
    int len = strlen(txt);
    if (j == Justification::CENTER) {
        w = (int)((double)(IC_WIDTH_PX/2.0) - ((double)get_ascii_size(txt, &this->std)/2.0));
    }
    for (int i = 0; i < len; i++) {
        w += draw_char_small(txt[i], w, start_y, is_bold, is_highlighted) + 1;
        if (w >= IC_WIDTH_PX) {
            return;
        }
    }
}

void kombi_lcd::draw_text_large(char *txt, int row, Justification j, bool is_bold, bool is_highlighted) {
    int start = 1;
    if (j == Justification::CENTER) {
        start = (int)((double)(IC_WIDTH_PX/2.0) - (double)get_ascii_size(txt, &this->large)/2.0);
    }
    int len = strlen(txt);
    for (int i = 0; i < len; i++) {
        start += draw_char_large(txt[i], start, row*12, is_bold, is_highlighted) + 1;
    }
}

int kombi_lcd::get_ascii_size(char* txt, ascii_table* t) {
    int total = 0;
    int len = strlen(txt);
    for(int i = 0; i < len; i++) {
        total += t->get_buffer(txt[i])->draw_width;
    }
    return total + strlen(txt)-1; // add the end so we add the pixel spacing between chars
}

void kombi_lcd::toggle_cc_display(bool state) {
    this->show_cc = state;
}

#define WIDTH_CHAR 12
void kombi_lcd::draw_gear_display(bool p, bool r, bool n, bool d, char custom_d) {

    if (p) {
        this->draw_box(4, IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX - 20, 14, 9, true);
    }
    this->draw_ascii('P', &this->large_bold, 5, IC_HEIGHT_TOP_PX+IC_HEIGHT_BOT_PX - 8, p);

    if (r) {
        this->draw_box(4+WIDTH_CHAR, IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX - 20, 14, 9, true);
    }
    this->draw_ascii('R', &this->large_bold, 5 + WIDTH_CHAR, IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX - 8, r);

    if (n) {
        this->draw_box(4+2*WIDTH_CHAR, IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX - 20, 14, 9, true);
    }
    this->draw_ascii('N', &this->large_bold, 5 + (2 * WIDTH_CHAR), IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX - 8, n);

    if (d) {
        this->draw_box(4+3*WIDTH_CHAR, IC_HEIGHT_TOP_PX + IC_HEIGHT_BOT_PX - 20, 14, 9, true);
    }
    this->draw_ascii(custom_d, &this->large_bold, 5+(3*WIDTH_CHAR), IC_HEIGHT_TOP_PX+IC_HEIGHT_BOT_PX - 8, d);
}

// (x,y) - Top left of box
void kombi_lcd::draw_box(int x, int y, int h, int w, bool filled) {
    // y_pos - bottom of the row
    // x_pos - left of the image
    for (int y_pos = y; y_pos < y + h; y_pos++) {
        if (y_pos >= 0 && y_pos < IC_HEIGHT_TOP_PX+IC_HEIGHT_BOT_PX) {
            for (int x_pos = x; x_pos < x + w; x_pos++) {
                if (x_pos >= 0 && x_pos < IC_WIDTH_PX) {
                    this->px_states[(IC_WIDTH_PX * y_pos) + x_pos] = true; //  TODO - OUTLINED  BOX
                }
            }
        }
    }
}

