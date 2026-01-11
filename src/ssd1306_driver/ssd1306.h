/*
 ******************************************************************************
 File:     ssd1306.h
 Info:     SSD1306 Oled display driver for stm32f10x MCU (It uses to frame buffer)

 The MIT License (MIT)
 Copyright (c) 2019 M.Cetin Atila

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

 ******************************************************************************
 */

#ifndef SSD1306_H
#define SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f10x.h"

#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64
#define SSD1306_I2C_ADDRESS 0x78

#define I2C_CON I2C1

//Display buffer
uint8_t display_buffer[(SCREEN_WIDTH*SCREEN_HEIGHT)/8];


/*
 * @brief Initialize SSD1306 display
 * @return 0 if successful
 */
uint8_t SSD1306_Init(void);


/*
 * @brief Clear display buffer (set all pixels to OFF)
 *        This only clears the software buffer, call SSD1306_Flush() to update display
 */
void SSD1306_Clear(void);

/*
 * @brief Flush display buffer to SSD1306 RAM (update display)
 * @return 0 if successful, error code if failed
 * @note Error codes:
 *       1: I2C START condition failed
 *       2: I2C address transmission failed
 *       3: Control byte transmission failed
 *       4: Command byte transmission failed
 */
uint8_t SSD1306_Flush(void);


/*
 * @brief Control display power state
 * @param power_mode: 1 = display ON, 0 = display OFF
 */
void SSD1306_Power(uint8_t power_mode);


/*
 * @brief Set individual pixel state in display buffer
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 * @param state: 1 = pixel ON, 0 = pixel OFF
 */
void SSD1306_Set_Pixel(uint16_t x, uint16_t y, uint8_t state);



#ifdef __cplusplus
	}//extern "C"
#endif

#endif
