/*
 * disp_fgv.h
 *
 *  Created on: 2020. aug. 14.
 *  Author: Póti Szabolcs
 */

#ifndef DISP_FGV_H_
#define DISP_FGV_H_

#include "Fonts_and_bitmaps_FLASH.h"

#define Overwrite	0x00
#define Merge		0x01

#define Pixel_on	0x80
#define Pixel_off	0x7f

#define size_5x8	0x08
#define size_10x16	0x16

#define ALIGN_LEFT		0x01
#define ALIGN_RIGHT		0x00

#define lcd_err			0xff


void delete_disp_mat(void);
void print_disp_mat(void);
void character_info(char characterf, uint8_t size);

//____________________for vertical orientation________________________________//
uint8_t write_character_V(uint8_t start_col, uint8_t start_row, char character, uint8_t Pixel_Status, uint8_t size);
uint8_t write_text_V(uint8_t col, uint8_t row, char *text, uint8_t Pixel_Status, uint8_t size);
uint8_t write_dec_num_uint16_t_V(uint8_t col, uint8_t row, uint16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_int16_t_V(uint8_t col, uint8_t row, int16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_uint8_t_V(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_time_format_V(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_float_V(uint8_t col, uint8_t row, float num, uint8_t digits_after_dot, uint8_t Pixel_Status, uint8_t size);
void print_bmp_V(uint8_t col, uint8_t row, uint8_t constant *bmp, uint8_t Pixel_Status, uint8_t write_mode);
uint8_t write_dec_num_uint32_t_V(uint8_t col, uint8_t row, uint32_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);

//____________________for horizontal orientation________________________________//
uint8_t write_character_H(uint8_t start_col, uint8_t start_row, char character, uint8_t Pixel_Status, uint8_t size);
uint8_t write_text_H(uint8_t col, uint8_t row, char *text, uint8_t Pixel_Status, uint8_t size);
uint8_t write_dec_num_uint16_t_H(uint8_t col, uint8_t row, uint16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_int16_t_H(uint8_t col, uint8_t row, int16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_uint8_t_H(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_time_format_H(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align);
uint8_t write_dec_num_float_H(uint8_t col, uint8_t row, float num, uint8_t digits_after_dot, uint8_t Pixel_Status, uint8_t size);
void print_bmp_H(uint8_t col, uint8_t row, uint8_t constant *bmp, uint8_t Pixel_Status, uint8_t write_mode);

void setpixel(uint8_t x, uint8_t y, uint8_t Pixel_status);
void draw_rectangle_xy_height_width(uint8_t x, uint8_t y, uint8_t rect_width, uint8_t rect_height, uint8_t pixel_status);
void draw_rectangle_x1y1_x2y2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pixel_status);
void draw_line_x(uint8_t x1, uint8_t x2,uint8_t y, uint8_t pixel_status);
void draw_line_y(uint8_t y1, uint8_t y2,uint8_t x, uint8_t pixel_status);
void fill_rectangle_x1y1_x2y2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pixel_status);
void fill_rectangle_xy_height_width(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t pixel_status);

#endif //DISP_FGV_H_
