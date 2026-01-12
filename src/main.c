/*
 ******************************************************************************
 File:     main.c
 Info:     Font test application

 The MIT License (MIT)
 Copyright (c) 2026 Cetin Atila

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


// Data types
#include <stdint.h>

// For sprintf
#include <stdio.h>

// STM32 peripheral library
#include "stm32f10x.h"

// SSD1306 Oled display driver
#include "ssd1306_driver/ssd1306.h"

// Include LFC font library
#include "lfc_font_lib/lfc_font.h"

// Include examples
#include "examples/examples.h"


// Include fonts
#include "fonts/C8_fonts.h"








void I2C1_Init(uint32_t speed) {

	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* GPIOB peripheral clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// I2C1 and I2C2 peripheral clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	// Configure I2C1 pins: SCL and SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 =0x10;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = speed;

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);

}




uint8_t Device_Display_1306(DISPLAY_CONTEXT * device_display){

	if(device_display==NULL){
		return 1;
	}

	// Set display properties (for ssd1306)
	device_display->width		    = 128;    // Display width
	device_display->height		    = 64;     // Display height
	device_display->rotation	    = 0;	             // Screen rotation (0-3), 0: 0, 1:90, 2:180, 3:270 degree
	device_display->set_pixel_func  = SSD1306_Set_Pixel; // Pixel write function, it defined in display library

	return 0;
}







int main(void) {

	// I2C speed max:400000
	uint32_t i2c_speed=400000;

	// STM32 I2C1 initialize
	I2C1_Init(i2c_speed);

	// SSD oled initialize
	SSD1306_Init();

	// Display context
	DISPLAY_CONTEXT main_display;

	// Set display
    Device_Display_1306(&main_display);

	// Clear display
	SSD1306_Clear();

	// Flush display buffer
	SSD1306_Flush();

	// Examples
	// ======================================================================================

	//Simple_Print_Text(&main_display);

	//Text_With_Extra_Space(&main_display);

	Text_With_Boundary_Box(&main_display);

}

