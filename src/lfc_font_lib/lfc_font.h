/*
 *
 *  File:     lfc_font.h
 *  Info:     LFC font library for embedded systems
 *
 */




/*
 ******************************************************************************

 The MIT License (MIT)
 Copyright (c) 2026 M.Cetin Atila

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




#ifndef LFC_FONT_H
#define LFC_FONT_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stddef.h>


// Set pixel function prototype
typedef void ( *CB_Set_Pixel)(uint16_t x,uint16_t y,uint8_t c);


// Define missing char width and height calculation
#define MISSING_CHAR_SIZE(font_height) (((uint8_t)(font_height)*2)/3)


// Used by the draw rectangle function
#define LFC_FILL 1
#define LFC_LINE 0

// Print form configuration properties
#define LFC_SPACING    		 0X40 // Use extra spacing
#define LFC_INVERT     		 0X20 // Invert pixel
#define LFC_BOUNDING_BOX     0X10 // Show bounding box
#define LFC_DEFAULT_CONFIG   0X00 // Default text configurations

// C8 font signature
#define LFC_C8_FONT_SIGNATURE  0XC8
#define LFC_C8_FONT_HEADER_LEN 0X05


// Use structure for computing string bounding box
typedef struct{
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t height;
}LFC_RECT;


// Display properties
typedef struct{
	uint16_t width;      // Screen width before rotation
	uint16_t height;	 // Screen height before rotation
	uint8_t  rotation;   // User-defined display rotation 0-3
	CB_Set_Pixel set_pixel_func; // Set pixel function (defined in the display driver)
}DISPLAY_CONTEXT;


// Print format structure
// Contains font and print properties
// Used by the print function
typedef struct{
	const uint8_t * font;  	// Font array (C8 format)
	uint8_t config;		    // Properties, include: invert,spacing,boundary box
	int8_t spacing;		    // Space between characters if set spacing from config
	int8_t padding;	        // Boundary box padding size, all directions between text and box line
	DISPLAY_CONTEXT *display_context;
}PRINT_FORM;



/*
 * @brief Calculates the bounding rectangle for a string without rendering it
 *
 * @param font: Font properties
 * @param str: UTF-8 encoded input string
 * @param x: Starting X coordinate
 * @param y: Starting Y coordinate
 * @param rect: Output rectangle structure
 *
 * @return: 0 on success, 1 on error
 *
 */
uint8_t LFC_Str_Rect(PRINT_FORM *print_form, const uint8_t * str,int16_t x,int16_t y,LFC_RECT *rect);


/*
 * @brief Renders UTF-8 encoded string to display
 *
 * @param print_form: Font properties
 * @param str: UTF-8 encoded input string
 * @param pos_x: Starting X coordinate
 * @param pos_y: Starting Y coordinate
 *
 * @return: Final X coordinate after rendering complete string (next caret position)
 *
 * This is the main string rendering function that processes UTF-8 strings,
 * handles special characters (space, newline), converts to UTF-32,
 * looks up characters in font, and renders them sequentially.
 */

int16_t LFC_Print(PRINT_FORM * print_form, const uint8_t * str, int16_t pos_x, int16_t pos_y);

/*
 * @brief Calculate string width without rendering
 *
 * @param print_form: Font properties
 * @param str: UTF-8 encoded input string
 *
 * @return: Width of the string in pixel
 *
 */
uint16_t LFC_Str_Width(PRINT_FORM * print_form, const uint8_t *str);


/*
 * @brief Draws a rectangle on the display with optional filling
 *
 * @param display_context: Display properties
 * @param rect: Rectangle structure defining position and size
 * @param fill: 1 to fill rectangle, 0 for outline only
 * @param invert: Invert drawing pixel
 *
 * @return: 0 if successful, 1 if rectangle is completely off-screen
 *
 */

uint8_t LFC_Draw_Rect(DISPLAY_CONTEXT * display_context, LFC_RECT * rect, uint8_t fill, uint8_t invert);


/*
 * @brief Copy print form
 *
 * @param dest: Source print form structure
 * @param src : Destination print form structure
 *
 * @return: 0:Success, 1:Error NULL pointer
 *
 */

uint8_t LFC_Copy_Print_Form(PRINT_FORM * dest,const PRINT_FORM * src);


/*
 * @brief Renders UTF-32 code character
 *
 * @param print_form: Font properties
 * @param str: UTF-32 character code
 * @param pos_x: Starting X coordinate
 * @param pos_y: Starting Y coordinate
 *
 * @return: Final X coordinate after rendering complete string (next caret position)
 *
 */

int16_t LFC_Print_Utf32_Chr(PRINT_FORM *print_form,uint32_t chr,int16_t pos_x,int16_t pos_y);




uint8_t LFC_Utf32_Char_Rect(PRINT_FORM *print_form, const uint32_t chr,int16_t x,int16_t y,LFC_RECT *rect);



/*
 * @brief Font validation function
 *
 * @param font: C8 font array
 *
 * @return: 0: Success
 * 			1: Error NULL pointer
 * 			2: Error signature, format error
 * 			3: Error structure length
 * 			4: Error offset address
 *
 */

uint8_t  LFC_Check_Font(const uint8_t * font);


#ifdef __cplusplus
	}//extern "C"
#endif



#endif /* LFC_FONT_H */
