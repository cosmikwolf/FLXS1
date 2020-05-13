/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define init_board(g)          SSD1351_init_board()
#define post_init_board(g)     SSD1351_post_init_board()
#define setpin_reset(g, state) SSD1351_setpin_reset(state)
#define acquire_bus(g)         SSD1351_aquirebus()
#define release_bus(g)         SSD1351_releasebus()
#define write_cmd(g, cmd)      SSD1351_write_cmd(cmd)
#define write_data(g, data)    SSD1351_write_data(data)
#define set_backlight(g, perent)
#define setreadmode(g)
#define setwritemode(g)
#define read_data(g) 0

#ifdef __cplusplus
extern "C" {
#endif

void SSD1351_init_board(void);
void SSD1351_post_init_board(void);
void SSD1351_setpin_reset(int state);
void SSD1351_aquirebus(void);
void SSD1351_releasebus(void);
void SSD1351_write_cmd(uint8_t cmd);
void SSD1351_write_data(uint8_t data);

#ifdef __cplusplus
}
#endif



#endif /* _GDISP_LLD_BOARD_H */
