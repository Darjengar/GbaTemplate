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

OBJ_ATTR obj_buffer[128];

/* -- Prototypes --*/
void load_gfx();

/* -- Methods -- */

/**
 *   @brief        brief Entry Point of GBA game
 */
int main(void)
{
    load_gfx();

    oam_init(obj_buffer, 1);
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

    int x = 80, y = 80;
    u32 pb = 0, tid = 512;
    OBJ_ATTR *test_obj = &obj_buffer[0];

    obj_set_attr(
        test_obj,
        ATTR0_SQUARE | ATTR0_HIDE,
        ATTR1_SIZE_32, 
        ATTR2_PALBANK(pb) | tid);

    obj_set_pos(test_obj, x, y);
    oam_copy(oam_mem, obj_buffer, 1);

    int txt_pal_idx = 2;
    int frame = 0;
    // txt_delay should have a value which is a power of two
    int txt_delay = 32;
    int START_PAL_3 = 48;
    COLOR orig_clr = pal_bg_mem[txt_pal_idx];
    COLOR curr_clr = orig_clr;
    COLOR new_clr = orig_clr;
    int curr_chn_num = 0;
    int curr_chn = bf_get(curr_clr, 0, 5);

    pal_bg_mem[START_PAL_3] = orig_clr;
    pal_bg_mem[START_PAL_3 + 1] = curr_chn << 0;
    pal_bg_mem[START_PAL_3 + 2] = CLR_MONEYGREEN;

    m4_rect(0, 0, 16, 16, START_PAL_3);
    m4_rect(M4_WIDTH - 16, 0, M4_WIDTH, 16, START_PAL_3 + 1);

    vid_flip();

    m4_rect((M4_WIDTH / 2) - 8, (M4_HEIGHT / 2) - 8, (M4_WIDTH / 2) + 8, (M4_HEIGHT / 2) + 8, START_PAL_3 + 2);

    while (TRUE)
    {
        vid_vsync(); 
        key_poll();

        if ((frame & txt_delay) == 0)
        {
            pal_bg_mem[txt_pal_idx] = CLR_BLACK;
        }
        else
        {
            pal_bg_mem[txt_pal_idx] = curr_clr;
            pal_bg_mem[START_PAL_3] = new_clr;
        }

        if (key_hit(KEY_START))
        {
            vid_flip();
            //obj_unhide(test_obj, ATTR0_REG);
            
            while (TRUE)
            {
                key_poll();

                if (key_hit(KEY_START)) 
                {
                    break;
                }
            }
            vid_flip();
        }

        if (key_hit(KEY_A))
        {
            curr_clr = new_clr;
            pal_bg_mem[txt_pal_idx] = curr_clr;
        }
        else if (key_hit(KEY_B))
        {

        }
        else if (key_hit(KEY_L))
        {
            curr_chn_num = clamp(--curr_chn_num, 0, 3);
            curr_chn = bf_get(new_clr, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
        }
        else if (key_hit(KEY_R)) 
        {
            curr_chn_num = clamp(++curr_chn_num, 0, 3);
            curr_chn = bf_get(new_clr, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
        }
        if (key_hit(KEY_UP))
        {
            txt_delay--;
        }
        else if (key_hit(KEY_DOWN))
        {
            txt_delay++;
        }
        else if (key_held(KEY_LEFT))
        {
            curr_chn = clamp(--curr_chn, 0, 32);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
            new_clr = bf_merge(new_clr, curr_chn, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3] = new_clr;
        }
        else if (key_held(KEY_RIGHT))
        {
            curr_chn = clamp(++curr_chn, 0, 32);
            pal_bg_mem[START_PAL_3 + 1] = curr_chn << (5 * curr_chn_num);
            new_clr = bf_merge(new_clr, curr_chn, 5 * curr_chn_num, 5);
            pal_bg_mem[START_PAL_3] = new_clr;
        }
        frame++;
    }

    return 0;
}

void load_gfx()
{
    memcpy(&vid_mem_back[0], test_bgBitmap, test_bgBitmapLen);
    memcpy(pal_bg_mem, test_bgPal, test_bgPalLen);
    memcpy(&tile_mem[5][0], player2Tiles, player2TilesLen);
    memcpy(pal_obj_mem, player2Pal, player2PalLen);
}

// EOF