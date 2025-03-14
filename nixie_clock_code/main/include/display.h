/*
Copyright (c) 2020 Tony Pottier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@file display.h
@author Tony Pottier

Contains functions to control the nixie clock display over SPI

*/


#ifndef MAIN_DISPLAY_H_
#define MAIN_DISPLAY_H_

#include <time.h>
#include <stdbool.h>
#include "ws2812.h"

#ifdef __cplusplus
extern "C" {
#endif



#define CATHODE_ON 1
#define CATHODE_OFF 0
#define ANODE_ON 1
#define ANODE_OFF 0

#define CATHODES_CNT 10
#define ANDODES_CNT 4

#define DOT_PIN 22


#define C0_PIN 16
#define C1_PIN 17
#define C2_PIN 23
#define C3_PIN 21
#define C4_PIN 18
#define C5_PIN 5
#define C6_PIN 2
#define C7_PIN 27
#define C8_PIN 4
#define C9_PIN 19

#define A0_PIN 26
#define A1_PIN 25
#define A2_PIN 32
#define A3_PIN 33

#define DYNAMIC_INDICATION_TIME_MS 2

// #define	DISPLAY_SPI_CS_GPIO				15
// #define DISPLAY_SPI_MOSI_GPIO			13
// #define DISPLAY_SPI_SCLK_GPIO			14
// #define DISPLAY_OE_GPIO					27
// #define DISPLAY_HVEN_GPIO				26
// #define DEBUG_USB_POWER_ON_GPIO			17

// #define DISPLAY_DIGIT_COUNT				6

// #define DISPLAY_TOP_DOT_MASK			(uint16_t)(1<<10)
// #define DISPLAY_BOTTOM_DOT_MASK			(uint16_t)(1<<11)



// typedef enum display_leading_zero_t{
// 	DISPLAY_LEADING_ZERO_HIDE = 0,
// 	DISPLAY_LEADING_ZERO_SHOW = 1
// }display_leading_zero_t;

// typedef enum display_dot_mode_t{
// 	DISPLAY_DOT_MODE_BLINK_ON_EVEN_SECONDS = 0,
// 	DISPLAY_DOT_MODE_BLINK_ON_ODDS_SECONDS = 1,
// 	DISPLAY_DOT_MODE_BLINK_LEFT_RIGHT = 2,
// 	DISPLAY_DOT_MODE_BLINK_WHEEL_CW = 3,
// 	DISPLAY_DOT_MODE_PM_INDICATOR = 4,
// 	DISPLAY_DOT_MODE_PM_INDICATOR_BLINK = 5,
// 	DISPLAY_DOT_MODE_OFF = 0x7fffffff
// }display_dot_mode_t;

// typedef struct display_config_t{
// 	display_dot_mode_t dot_mode;
// 	display_leading_zero_t leading_zero;
// 	bool twelve_hours_format;
// 	float led_brightness;
// 	rgb_t led_color;
// }display_config_t;

// esp_err_t display_init();
// esp_err_t display_write_time(struct tm *time);
// uint16_t* display_get_vram();
// esp_err_t display_write_vram();
// esp_err_t display_register_usb_power_interrupt();

// /**
//  * @brief set the clock display configuration that will be used while displaying the time
//  * @see display_write_time
//  */
// void display_set_config(display_config_t* config);

// void display_turn_on();
// void display_turn_off();


// typedef struct display_config_t{
// 	display_dot_mode_t dot_mode;
// 	display_leading_zero_t leading_zero;
// 	bool twelve_hours_format;
// 	float led_brightness;
// 	rgb_t led_color;
// }display_config_t;

void display_init(void);
void display_set(int h,int m);
void display_set_dots(bool state);
void display_on(void);
void display_off(void);


#ifdef __cplusplus
}
#endif

#endif /* MAIN_DISPLAY_H_ */
