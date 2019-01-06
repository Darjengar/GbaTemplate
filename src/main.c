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
#include <agb_debug.h>

/* -- Prototypes --*/
void load_gfx();

struct ColorComponents {
    u16 r : 5;
    u16 g : 5;
    u16 b : 5;
    u16 unused : 1;
};

union Color {
    struct ColorComponents components;
    u16 packed;
};

/* -- Methods -- */

/**
 *   @brief        brief Entry Point of GBA game
 */
int main(void)
{
    load_gfx();
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;

    int txt_pal_idx = 2;
    int frame = 0;
    int threshold = 16;
    int START_PAL_3 = 48;
    COLOR orig_clr = pal_bg_mem[txt_pal_idx];
    COLOR curr_clr = orig_clr;
    
    int CHANNEL_R_MASK = BF_MASK(0, 5);
    int CHANNEL_R_SHIFT = 0;
    int CHANNEL_G_MASK = BF_MASK(5, 5);
    int CHANNEL_B_MASK = BF_MASK(10, 5);

    int curr_chn_num = 0;
    int curr_chn = bf_get(curr_clr, 0, 5);

    pal_bg_mem[START_PAL_3] = orig_clr;
    pal_bg_mem[START_PAL_3 + 1] = curr_chn << 0;
    m4_rect(0, 0, 16, 16, START_PAL_3);
    m4_rect(M4_WIDTH - 16, 0, M4_WIDTH, 16, START_PAL_3 + 1);
    vid_flip();

    while (TRUE)
    {
        vid_vsync();
        key_poll();

        if ((frame & threshold) == 0)
        {
            pal_bg_mem[txt_pal_idx] = CLR_BLACK;
        }
        else
        {
            pal_bg_mem[txt_pal_idx] = curr_clr;
            pal_bg_mem[START_PAL_3] = curr_clr;
        }

        if (key_hit(KEY_A))
        {
            pal_bg_mem[txt_pal_idx] = curr_clr;
        }
        else if (key_hit(KEY_B))
        {
            curr_clr = CLR_BLUE;
        }
        else if (key_hit(KEY_L))
        {
            curr_chn_num = clamp(--curr_chn_num, 0, 3);
            curr_chn = bf_get(curr_clr, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
        }
        else if (key_hit(KEY_R)) 
        {
            curr_chn_num = clamp(++curr_chn_num, 0, 3);
            curr_chn = bf_get(curr_clr, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
        }
        if (key_hit(KEY_UP))
        {
            threshold--;
        }
        else if (key_hit(KEY_DOWN))
        {
            threshold++;
        }
        else if (key_held(KEY_LEFT))
        {
            curr_chn = clamp(--curr_chn, 0, 32);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
            curr_clr = bf_merge(curr_clr, curr_chn, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3] = curr_clr;
        }
        else if (key_held(KEY_RIGHT))
        {
            curr_chn = clamp(++curr_chn, 0, 32);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
            curr_clr = bf_merge(curr_clr, curr_chn, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3] = curr_clr;
        }
        frame++;
    }

    return 0;
}

void load_gfx()
{
    memcpy(&vid_mem_back[0], &test_bgBitmap, test_bgBitmapLen);
    memcpy(&pal_bg_mem[0], &test_bgPal, test_bgPalLen);
}

// EOF