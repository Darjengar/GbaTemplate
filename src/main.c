/*
 * Copyright (C) 2017 DarkGengar <https://github.com/DarkGengar>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *   @file        main.c
 *   @date        25.10.2017
 *   @author      DarkGengar <https://github.com/DarkGengar>
 *   @brief       brief description
 *
 *   @details     More details about file.
 */

/* -- Includes -- */
#include <string.h>

#include <tonc.h>
#include <gfx.h>

/* -- Prototypes --*/
void load_gfx();

/* -- Methods -- */

/**
 *   @brief        brief Entry Point of GBA game
 */
int main(void) {

    load_gfx();
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;
    int txt_pal_idx = 2;
    COLOR red = RGB15(31, 0, 0);
    COLOR orig_clr = pal_bg_mem[txt_pal_idx];

    while (TRUE) {
	    vid_vsync();
        key_poll();
        if (key_is_down(KEY_A))
        {
            pal_bg_mem[txt_pal_idx] = red;
        }
        else
        {
            pal_bg_mem[txt_pal_idx] = orig_clr;
        }
    }

    return 0;
}

void load_gfx() {
    memcpy(&vid_mem_front[0], &test_bgBitmap, test_bgBitmapLen);
    memcpy(&pal_bg_mem[0], &test_bgPal, test_bgPalLen);
}

// EOF