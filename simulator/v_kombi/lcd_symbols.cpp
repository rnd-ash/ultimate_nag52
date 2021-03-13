//
// Created by ashcon on 2/22/21.
//

#include "lcd_symbols.h"

ascii_table::ascii_table(SDL_Renderer* r, char *path, int char_width, int char_height) {
    this->char_height = char_height;
    this->char_width = char_width;

    SDL_Surface *img = IMG_Load(path);
    SDL_Texture* t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, img->w, img->h);

    if (t) {
        void* pixels;
        this->bytes = new uint32_t[img->h*img->w];
        memset(this->bytes, 0x00, img->h*img->w * sizeof(uint32_t));
        SDL_LockTexture(t, &img->clip_rect, &pixels, &img->pitch);
        memcpy(this->bytes, img->pixels, img->w*img->h* sizeof(uint32_t));

        // Now to create our char maps
        int chars_x = img->w / char_width;
        int chars_y = img->h / char_height;

        int px_x = img->w;
        int px_y = img->h;

        printf("Char map loaded. Total size: %d x %d. Characters: %d x %d\n", img->w, img->h, chars_x, chars_y);
        SDL_UnlockTexture(t);
        SDL_DestroyTexture(t);


        int y_pos = 0;
        int x_pos = 0;

        // Build our char maps!
        int idx = 0;
        this->state_buffer = new lcd_char[chars_y*chars_x];
        for (int y = 0; y < chars_y; y++) {
            for (int x = 0; x < chars_x; x++) {
                this->state_buffer[idx] = lcd_char {
                    char_height,
                    char_width, // Default
                    char_width,
                    new bool[this->char_width*this->char_height],
                };
                int px_tmp = 0;
                int occupied_px = 0;
                bool* occupied_space = new bool[char_width];
                memset(occupied_space, 0x00, char_width*sizeof(bool));

                for (int i = y_pos; i < char_height + y_pos; i++) {
                    for (int l = x_pos; l < char_width+x_pos; l++) {
                        int colour = this->bytes[(i*px_x) + l];
                        if (colour == 0xFF000000) {
                            occupied_space[l-x_pos] = true;
                        }
                        this->state_buffer[idx].buf[px_tmp] = (colour == 0xFF000000);
                        px_tmp++;
                    }
                }

                bool is_occupied = false;
                for (int s = 0; s < char_width; s++){
                    if (occupied_space[s]) {
                        occupied_px++;
                    }
                }
                printf("%d %d\n", idx, occupied_px);
                if(occupied_px != 0) {
                    this->state_buffer[idx].draw_width = occupied_px;
                }
                y_pos += char_height;
                if (y_pos >= px_y) {
                    x_pos += char_width;
                    y_pos = 0;
                }
                idx++;
                delete[] occupied_space;
            }
        }

    }

}

ascii_table::~ascii_table() {
    delete[] this->bytes;
}

lcd_char* ascii_table::get_buffer(char c) {
    if (this->state_buffer == nullptr) {
        return nullptr;
    } else {
        int ascii = int((unsigned char)c);
        // LCD Cannot render these
        if (ascii < 32 || ascii > 127) {
            return nullptr;
        } else {
            // -32 so that ! is number 1 on our index
            return &this->state_buffer[ascii-32];
        }
    }
}

ascii_table::ascii_table() {
    this->bytes = nullptr;
    this->state_buffer = nullptr;
}

