#include <stdint.h>
#include <stdio.h>
#include "lfc_font.h"


#define GET_PIXEL(array,n)     ( ((array)[(n) >> 3])  &  (0x80>>((n)&7)) )


/**
 * @brief Converts UTF-8 character code to UTF-32 code (4 bytes)
 *
 * @param str: Input UTF-8 encoded string
 * @param u32_code: Output UTF-32 code (4 byte array)
 *
 * @return: UTF-8 byte length consumed, 0 if error
 *
 */

 uint8_t LFC_Utf8_To_Utf32(const uint8_t * str, uint8_t * u32_code) {

	// Validate input pointers
	if(str==NULL || u32_code==NULL){
		return 0;
	}

	uint8_t i = 0, len = 1, utf8_byte_count = 0;
	uint8_t u;


	const uint8_t u8[5] = { 0xFE, 0xFC, 0xF8, 0xF0, 0xE0 }; // Comparison masks
	const uint8_t c8[5] = { 0xFC, 0xF8, 0xF0, 0xE0, 0xC0 }; // Expected patterns

	// Clear UTF-32 output array (4 bytes)
	for (u = 0; u < 4; u++) {
		u32_code[u] = 0;
	}

	// Check if character is multi-byte UTF-8 (above ASCII range)
	if ((*str) > 127) {
		utf8_byte_count = 0;
		for (i = 0; i < 5; i++) {
			if ((*str & u8[i]) == c8[i]) {
				utf8_byte_count = 5 - i;
				u32_code[0] = (*str) & (0xFF >> (7 - i));
				break;
			}
		}
		if (utf8_byte_count == 0) {
			return 0;
		}
		// Process continuation bytes (10xxxxxx format)
		for (i = 0; i < utf8_byte_count; i++) {
			str++;
			len++;
			// Check for string termination
			if (*str == 0) {
				return 0;
			}
			// Validate continuation byte format (must start with 10)
			if ((*str & 0xC0) != 0x80) {
				return 0;
			}

			// Shift left 6-bit
			for (u = 3; u > 0; u--) {
				u32_code[u] <<= 6;
				u32_code[u] |= (u32_code[u - 1] >> 2);
			}
			u32_code[0] <<= 6;
			u32_code[0] |= ((*str) & 0x3F);
		}
	} else {
		// Single byte ASCII character - direct copy
		u32_code[0] = *str;
	}

	// Return total bytes consumed from input string
	return len;
}


/**
 * @brief Searches for UTF-32 character in font character map and returns its data offset
 * @param font: Pointer to font data array
 * @param u32_code: UTF-32 character code to search for
 * @param character_count: Total number of characters in font
 * @param cmap_start: Starting offset of character map in font data
 * @return: Offset to character data in font, 0 if not found
 *
 * This function performs a linear search through the font's character map
 * to find the matching UTF-32 code and retrieve the corresponding character
 * data offset for rendering.
 */


uint16_t LFC_Get_Chr_Index(const uint8_t * font, const uint8_t * u32_code, uint16_t character_count, uint8_t cmap_start) {

	// Check validate
	if(font==NULL || u32_code==NULL){
		return 0;
	}

	uint8_t i, u;
	uint16_t ind = 0;
	uint16_t cind = 0;

	// Iterate through all characters in font character map
	for (u = 0; u < character_count; u++) {
		// Calculate index for current character in map (6 bytes per entry)
		ind = cmap_start + u * 6;

		// Compare 4-byte UTF-32 code with font entry
		for (i = 0; i < 4; i++) {
			if (u32_code[i] != font[ind + i]){
				break;
			}
		}

		// If all 4 bytes matched, extract character data offset
		if (i == 4) {
			ind += i; // Move to offset bytes in character map entry
			cind = font[ind++]; // Read LSB of offset
			cind |= ((uint16_t) (font[ind++])) << 8; // Read MSB of offset
			return cind;
		}
	}

	return 0; // Character not found in the font
}

/**
 * @brief Renders a single character to the display
 *
 * @param font: Pointer to font data array
 * @param ind: Index to character data in font
 * @param cx: Current X coordinate for rendering
 * @param cy: Current Y coordinate for rendering
 * @param pixel_fnc: Callback function to set individual pixels
 *
 * @return: Next X coordinate after rendering the character ( x + string width)
 *
 * This function reads character bitmap data from font and renders it
 * pixel by pixel using the provided callback function. It handles
 * coordinate transformation and clipping automatically.
 */

static int16_t _LFC_Print_Chr(PRINT_FORM * print_form,uint16_t ind, int16_t cx, int16_t cy) {

	// Check validate
	if(print_form==NULL){
		return 0;
	}

	// Check validate
	if(print_form->font==NULL || print_form->display_context==NULL){
		return 0;
	}


	uint8_t invert=print_form->config & LFC_INVERT;

	const uint8_t *font=print_form->font;

	DISPLAY_CONTEXT *display_context = print_form->display_context;

	CB_Set_Pixel set_pixel_fnc = display_context->set_pixel_func;

	// Check validate
	if(set_pixel_fnc==NULL){
		return 0;
	}

	// Display context properties
	uint16_t screen_width;
	uint16_t screen_height;
	uint8_t  screen_rotation  = display_context->rotation;


	if((screen_rotation&0x1) == 0x01){
		screen_height     = display_context->width; //change width to height
		screen_width      = display_context->height; //change height to width
	}else{
		screen_width      = display_context->width;
		screen_height     = display_context->height;
	}


	int16_t x = 0, y = 0;

	uint8_t reverse_x=0;
	uint8_t reverse_y=0;


	// 180 degree and 270 degree
	if((screen_rotation&0x03)==0x02 || (screen_rotation&0x03)==0x03){
		reverse_x=1;
	}

	// 0 degree and 270 degree
	if((screen_rotation&0x03)==0x00 || (screen_rotation&0x03)==0x03){
		reverse_y=1;
	}


	// Character bitmap info
	uint8_t bitmap_height;
	uint8_t bitmap_width;
	int8_t  bitmap_top;
	int8_t  bitmap_left;
	uint8_t advance;
	uint8_t bit_status;


	// Read character properties from character header
	bitmap_width  = font[ind++]; // Bitmap width in pixels
	bitmap_height = font[ind++]; // Bitmap height in pixels
	bitmap_top    = font[ind++]; // Bitmap top in pixel from origin
	bitmap_left	  = font[ind++]; // Bitmap left in pixel from origin
	advance       = font[ind++]; // character width


	cx +=bitmap_left;

	int16_t min_x,max_x;
	int16_t min_y,max_y;


	if(reverse_x){
		int16_t rx = screen_width  - cx;
		min_x=rx-bitmap_width;
		max_x=rx;
	}else{
		min_x=cx;
		max_x=cx+bitmap_width;
	}

	int16_t chr_top=bitmap_height-bitmap_top;
	if(reverse_y){
		int16_t ry = screen_height - cy + chr_top;
		min_y=ry - bitmap_height;
		max_y=ry;
	}else{
		int16_t ry = cy - chr_top;
		min_y=ry;
		max_y=ry + bitmap_height;
	}

	//Check if character is completely outside screen horizontally
	if (((max_x < 0) && (min_x < 0))||((max_x > screen_width) && (min_x > screen_width))) {

		if(advance>bitmap_width){
			return cx + advance;
		}
		return cx + bitmap_width;
	}

	//Check if character is completely outside screen vertically
	if (((max_y < 0) && (min_y < 0))||((max_y > screen_height) && (min_y > screen_height))) {

		if(advance>bitmap_width){
			return cx + advance;
		}
		return cx + bitmap_width;
	}


	// This code would clip characters at screen boundaries
	// Disabled for performance
	/*
	if(max_x>screen_width){
		max_x=screen_width;
	}
	if(min_x<0){
		min_x=0;
	}

	if(max_y>screen_height){
		max_y=screen_height;
	}
	if(min_y<0){
		min_y=0;
	}
	*/


	// Iterate through each pixel in character bitmap
	for (y = min_y; y < max_y; y++) {

		// Check reserve y axis
		uint16_t fy;
		if(!reverse_y){
			fy = max_y - y - 1;
		}else{
			fy = y - min_y;
		}

		for (x = min_x; x < max_x; x++) {

			// Check reserve x axis
			uint16_t fx;
			if(reverse_x){
				fx = max_x - x - 1;
			}else{
				fx = x - min_x;
			}

			// Extract pixel value from bitmap data (1 bit per pixel)
			bit_status = GET_PIXEL((&font[ind]), (fy * bitmap_width + fx));

			// Apply inversion if requested
			if (invert) {
				bit_status = !bit_status;
			}

			// For kerning
			if(bit_status==0 && invert==0) continue;

			// Set pixel using callback function
			if(screen_rotation&0x01){ // Change x and y axis
				set_pixel_fnc(y, x, bit_status); // 90 and 270 degrees
			}else{
				set_pixel_fnc(x, y, bit_status); // 0 and 180 degrees
			}

		}
	}

	// Next cursor position
	if(advance>bitmap_width){
		return cx + advance;
	}
	return cx + bitmap_width;

}





static int16_t _LFC_Print(PRINT_FORM * print_form, const uint8_t * str, int16_t x, int16_t y) {

	// Check validate
	if(print_form==NULL || str==NULL){
		return 1;
	}

	// Check validate
	if(print_form->display_context==NULL || print_form->font==NULL){
		return 1;
	}


	const uint8_t * font=print_form->font;
	uint8_t u32_code[4];
	int16_t px, py;
	px = x;
	py = y;

	const uint8_t* s = (uint8_t *)str;

	// Font header variables
	uint16_t ind, cind;
	uint8_t cmap_start;
	uint8_t font_height;
	uint8_t font_signature;
	uint8_t header_size;
	uint16_t character_count;


	// Parse font header to get font properties
	ind = 0;
	font_signature = font[ind++];
	if (font_signature != 0xC8) {
		return 0; // Invalid font signature
	}

	// Read font header structure
	header_size = font[ind++];		// Header size in bytes
	font_height = font[ind++];		// Font height in pixels

	// Character count (stored as little-endian 16-bit value)
	character_count = font[ind++];
	character_count |= font[ind++] << 8;

	//Character map starts immediately after header
	cmap_start = header_size;

	// Process string character by character
	while (*s) {

		// Handle newline character
		if(*s=='\n'){
			px=x;              // Return to left edge
			py-=font_height+1; // Move to next line
			s++;
			continue;
		}

		// Convert UTF-8 to UTF-32 for font lookup
		uint8_t u8_len = LFC_Utf8_To_Utf32(s, u32_code);
		if (u8_len == 0) {
			return 0; // Conversion error
		} else {
			s += u8_len;
		}

		// Look up character in font character map
		cind = LFC_Get_Chr_Index(font, u32_code, character_count, cmap_start);

		if (cind) {
			// Render character and advance X position
			if(print_form->config & (LFC_SPACING)){
				px = _LFC_Print_Chr(print_form,cind, px, py)+print_form->spacing;
			}else{
				px = _LFC_Print_Chr(print_form,cind, px, py);
			}

		}else{
			// Unknown character; we use a rectangle instead of it.
			uint8_t missing_char_width=MISSING_CHAR_SIZE(font_height);
			if(missing_char_width>5){
				LFC_RECT rect;
				rect.x = px + 2;
				rect.y = py;
				rect.width  = missing_char_width-4;
				rect.height = missing_char_width;
				LFC_Draw_Rect(print_form->display_context,&rect,LFC_LINE,0);
			}
			px += missing_char_width; // Calculate space
		}
	}

	return px; // Return next X position
}

/*
 * @brief Calculates the bounding rectangle for a string without rendering it
 *
 * @param print_form: Font properties
 * @param str: UTF-8 encoded input string
 * @param x  : Starting X coordinate
 * @param y  : Starting Y coordinate
 * @param padding: Additional padding around the text
 * @param rect: Output rectangle structure
 *
 * @return: 0 on success, 1 on error
 *
 * This function calculates the exact dimensions that a string would occupy
 * if rendered, useful for text layout, centering, and background drawing.
 */
uint8_t LFC_Str_Rect(PRINT_FORM * print_form,const uint8_t * str, int16_t x, int16_t y,LFC_RECT *rect) {

	if(print_form==NULL || str==NULL || rect==NULL){
		return 1;
	}

	if(print_form->display_context==NULL || print_form->font==NULL){
		return 1;
	}

	uint8_t u32_code[4];
	uint8_t* s = (uint8_t *)str;

	const uint8_t * font=print_form->font;
	uint8_t padding     =print_form->padding;


	// Font variables
	uint16_t ind, cind;
	uint8_t  cmap_start;
	uint8_t  font_signature;
	uint8_t  header_size;
	uint8_t  font_height;
	uint16_t character_count;

	int16_t px, py;
	px = x;
	py = y;

	int16_t min_y, max_y;


	int16_t rect_width   = 0;

	uint8_t chr_width    = 0;
	uint8_t chr_height   = 0;
	int8_t  chr_top      = 0;
    int8_t  chr_left     = 0;
	uint8_t chr_advance  = 0;



	// Parse font header (same as rendering function)
	ind = 0;
	font_signature = font[ind++];
	if (font_signature != 0xC8) {
		return 1;
	}

	header_size = font[ind++];
	font_height = font[ind++];

	character_count = font[ind++];
	character_count |= font[ind++] << 8;

	cmap_start = header_size;

	// Initialize rectangle
	rect->height = 0;
	rect->width = 0;
	rect->x = px;
	rect->y = py;

	min_y = 32000; // Initialize with big value
	max_y = py;


	int16_t max_width=0;


	// Calculate dimensions for each character
	while (*s) {

		// Handle newline character
		if(*s=='\n'){
			py-=font_height+1; // Move to next line
			s++;

			// Remove last space in the end of line
			if(print_form->config & (LFC_SPACING)){
				rect_width -= print_form->spacing;
			}

			if(max_width<rect_width){
				max_width=rect_width;
			}
			rect_width=0;

			continue;
		}

		// Convert UTF-8 to UTF-32
		uint8_t u8_len = LFC_Utf8_To_Utf32(s, u32_code);
		if (u8_len == 0) {
			return 0;
		} else {
			s += u8_len;
		}


		// Get character offset in the font array
		cind = LFC_Get_Chr_Index(font, u32_code, character_count, cmap_start);

		if (cind) {
			// Read character metrics from font
			chr_width   = font[cind++]; // Bitmap width
			chr_height  = font[cind++]; // Bitmap height
			chr_top     = font[cind++]; // Top offset
			chr_left    = font[cind++]; // Left offset
			chr_advance = font[cind++]; // character width


			if(chr_width>chr_advance){
				rect_width+=chr_width;
			}else{
				rect_width+=chr_advance;
			}

			if(print_form->config & (LFC_SPACING) ){
				rect_width += print_form->spacing;
			}

			rect_width += chr_left;

			// Calculate top Y coordinate of character
			int16_t base_line = py - (chr_height  - chr_top);

			// Update bounding box
			if (min_y > base_line) {
				min_y = base_line;
			}

			if (max_y < (base_line + chr_height)) {
				max_y = base_line + chr_height;
			}

		}else{
			// Unknown character
			uint8_t missing_char_width=MISSING_CHAR_SIZE(font_height);
			rect_width+=missing_char_width;

			if((missing_char_width+py)>max_y){
				max_y = missing_char_width + py;
			}
			if(min_y>py){
				min_y=py;
			}
		}
	}

	if(max_width<rect_width){
		max_width=rect_width;
	}

	// Set final rectangle dimensions
	rect->width = max_width;
	rect->y = min_y;
	rect->height = max_y - min_y;


	// Apply padding
	rect->width  += (2 * padding);
	rect->height += (2 * padding);

	rect->x -= padding;
	rect->y -= padding;

	return 0;
}



/*
 * @brief Draws a rectangle on the display with optional filling
 * @param rect: Rectangle structure defining position and size
 * @param fill: 1 to fill rectangle, 0 for outline only
 * @param invert: Invert drawing colors
 * @return: 0 if successful, 1 if rectangle is completely off-screen
 *
 * This function handles rectangle drawing with proper clipping to screen
 * boundaries and supports both filled and outline rendering modes.
 */

static uint8_t _LFC_Draw_Rect(DISPLAY_CONTEXT * display_context, LFC_RECT * rect, uint8_t fill, uint8_t invert) {

	if(rect==NULL || display_context==NULL){
		return 1;
	}


	CB_Set_Pixel set_pixel_fnc= display_context->set_pixel_func;

	if(set_pixel_fnc==NULL){
		return 1;
	}

	uint16_t screen_width  = display_context->width;
	uint16_t screen_height = display_context->height;


	int16_t x, y;
	int16_t max_x, min_x;
	int16_t max_y, min_y;

	uint8_t left_line   = 1;
	uint8_t right_line  = 1;
	uint8_t top_line    = 1;
	uint8_t bottom_line = 1;


	// Calculate rectangle boundaries
	min_x=rect->x;
	max_x=rect->x+rect->width;

	min_y=rect->y;
	max_y=rect->y+rect->height;


	//Check if rectangle is completely outside screen horizontally
	if (((max_x < 0) && (min_x < 0))||((max_x > screen_width) && (min_x > screen_width))) {
		return 1;
	}

	//Check if rectangle is completely outside screen vertically
	if (((max_y < 0) && (min_y < 0))||((max_y > screen_height) && (min_y > screen_height))) {
		return 1;
	}


	// Clip rectangle to screen boundaries
	if(max_x > screen_width){
		max_x=screen_width;
		right_line = 0;
	}
	if(min_x < 0){
		min_x=0;
		left_line = 0;
	}


	if(max_y > screen_height){
		max_y=screen_height;
		top_line = 0;
	}
	if(min_y < 0){
		min_y=0;
		bottom_line = 0;
	}


	// Invert pixel
	invert=invert?0:1;

	// Fill entire rectangle
	if (fill==LFC_FILL) {
		for (y = min_y; y <max_y; y++) {
			for (x = min_x; x < max_x; x++) {
				set_pixel_fnc(x, y, invert);
			}
		}
	}

	// Draw rectangle outline only
	else {
		// Draw left and right vertical lines
		if (left_line) {
			for (y = min_y; y < max_y; y++) {
				set_pixel_fnc(min_x, y, invert);
			}
		}
		if (right_line) {
			for (y = min_y; y < max_y; y++) {
				set_pixel_fnc(max_x-1, y, invert);
			}
		}

		// Draw top and bottom horizontal lines
		if (bottom_line) {
			for (x = min_x; x < max_x; x++) {
				set_pixel_fnc(x, min_y, invert);
			}
		}
		if (top_line) {
			for (x = min_x; x < max_x; x++) {
				set_pixel_fnc(x, max_y-1, invert);
			}
		}
	}

	return 0;
}





uint8_t LFC_Draw_Rect(DISPLAY_CONTEXT * display_context, LFC_RECT * rect, uint8_t fill, uint8_t invert) {

	LFC_RECT t_rect;

	if(display_context==NULL || rect==NULL){
		return 1;
	}

	// Display context properties
	uint16_t screen_width;
	uint16_t screen_height;
	uint8_t  screen_rotation  = display_context->rotation;

	screen_width      = display_context->width;
	screen_height     = display_context->height;


	// Change x to y axis and width to height
	if(screen_rotation&0x01){
		t_rect.x=rect->y;
		t_rect.y=rect->x;
		t_rect.width=rect->height;
		t_rect.height=rect->width;
	}else{
		t_rect.x=rect->x;
		t_rect.y=rect->y;
		t_rect.width=rect->width;
		t_rect.height=rect->height;
	}


	// reverse y axis : 0 degree and 270 degree
	if((screen_rotation&0x03)==0x00 || (screen_rotation&0x03)==0x03){
		t_rect.y=screen_height-(t_rect.y + t_rect.height);
	}

	// reverse x axis :180 degree and 270 degree
	if((screen_rotation&0x03)==0x02 || (screen_rotation&0x03)==0x03){
		t_rect.x=screen_width-(t_rect.x + t_rect.width);
	}


	return _LFC_Draw_Rect( display_context, &t_rect,  fill,  invert);

}



/*
 * @brief Calculate string width without rendering
 *
 * @param print_form: Font properties
 * @param str: UTF-8 encoded input string
 *
 *
 * @return: Width of the string in pixel
 *
 */
uint16_t LFC_Str_Width(PRINT_FORM * print_form, const uint8_t *str) {

	// Check null pointer
	if(print_form==NULL || str==NULL){
		return 0;
	}

	LFC_RECT rect;
	LFC_Str_Rect(print_form,str,0,0,&rect);
	return rect.width;

}


/*
 * @brief Renders UTF-8 encoded string to display
 *
 * @param print_form: Font properties
 * @param str       : UTF-8 encoded input string
 * @param x         : Starting X coordinate
 *
 * @return          : Final X coordinate after rendering complete string ( x + string width)
 *
 * This is the main string rendering function that processes UTF-8 strings,
 * handles special characters (space, newline), converts to UTF-32,
 * looks up characters in font, and renders them sequentially.
 */

int16_t LFC_Print(PRINT_FORM * print_form, const uint8_t * str, int16_t pos_x, int16_t pos_y){

	// Check null pointer
	if(print_form==NULL || str==NULL){
		return 0;
	}
	if(print_form->display_context==NULL){
		return 0;
	}

	pos_x+=print_form->padding;
	pos_y+=print_form->padding;

	// If set boundary box draw it and show string
	if(print_form->config & LFC_BOUNDING_BOX){
		LFC_RECT rect;
		LFC_Str_Rect(print_form,str,pos_x,pos_y,&rect);

		uint8_t fill;
		if(print_form->config & LFC_INVERT){
			fill=1;
		}else{
			fill=0;
		}
		LFC_Draw_Rect(print_form->display_context,&rect, fill,0);
		return _LFC_Print(print_form,str, pos_x, pos_y)+2*print_form->padding;

	}

	// Show string Without boundary box
	return _LFC_Print(print_form,str, pos_x, pos_y);
}




/*
 * @brief Converts a UTF-32 character code to UTF-8 encoding
 *
 * @param utf32_c: UTF-32 character code to be converted
 * @param utf8_s: Pointer to buffer where UTF-8 encoded result will be stored (7 bytes)
 * @return: Number of bytes written to the output buffer (1-6 bytes)
 * @note: Handles Unicode character conversion from 32-bit format to variable-length UTF-8 encoding
 *        The output buffer must have at least 7 bytes of available space
 */
uint8_t LFC_Utf32_To_Utf8(uint32_t utf32_c, uint8_t* utf8_s) {
	uint32_t us8_len;
	uint32_t i;
	uint8_t mask = 0xFF;

	// Check null pointer
	if(utf8_s==NULL || utf32_c==0){
		return 0;
	}

	// Fill utf-8 array to zero
    for (i = 0; i < 7; i++) {
        utf8_s[i] = 0;
    }

    us8_len = 0;

    if (utf32_c < 0x80) {
        utf8_s[us8_len++] = utf32_c;
    }
    else if (utf32_c < 0x800) {
        mask = mask << 6;
        utf8_s[us8_len++] = (((utf32_c) >> 6) & 0x1F) | mask;
        utf8_s[us8_len++] = (utf32_c & 0x3F) | 0x80;
    }
    else if (utf32_c < 0x10000) {
        mask = mask << 5;
        utf8_s[us8_len++] = (((utf32_c) >> 12) & 0x0F) | mask;
        utf8_s[us8_len++] = (((utf32_c) >>  6) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (utf32_c & 0x3F) | 0x80;
    }
    else if (utf32_c < 0x200000) {
        mask = mask << 4;
        utf8_s[us8_len++] = (((utf32_c) >> 18) & 0x07) | mask;
        utf8_s[us8_len++] = (((utf32_c) >> 12) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (((utf32_c) >>  6) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (utf32_c & 0x3F) | 0x80;
    }
    else if(utf32_c < 0x4000000) {
        mask = mask << 3;
        utf8_s[us8_len++] = (((utf32_c) >> 24) & 0x03) | mask;
        utf8_s[us8_len++] = (((utf32_c) >> 18) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (((utf32_c) >> 12) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (((utf32_c) >>  6) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (utf32_c& 0x3F) | 0x80;
    }
    else if (utf32_c < 0x80000000) {
        mask = mask << 2;
        utf8_s[us8_len++] = (((utf32_c) >> 30) & 0x01) | mask;
        utf8_s[us8_len++] = (((utf32_c) >> 24) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (((utf32_c) >> 18) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (((utf32_c) >> 12) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (((utf32_c) >>  6) & 0x3F) | 0x80;
        utf8_s[us8_len++] = (utf32_c & 0x3F) | 0x80;
    }

    return us8_len;
}



/*
 * @brief Copy print form
 *
 * @param dest: Source print form structure
 * @param src : Destination print form structure
 *
 * @return: 0:Success, 1:Error NULL pointer
 *
 */

uint8_t LFC_Copy_Print_Form(PRINT_FORM * dest,const PRINT_FORM * src){
	// Validate
	if(dest==NULL || src==NULL){
		return 1;
	}

	// Copy fields
	dest->display_context = src->display_context;
	dest->font			  = src->font;
	dest->spacing		  = src->spacing;
	dest->config	      = src->config;
	dest->padding	      = src->padding;

	return 0;
}



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
uint8_t LFC_Check_Font(const uint8_t * font){
	uint16_t ind=0;
	uint16_t chr_count=0;
	if(font==NULL){
		return 1; // Error NULL pointer
	}


	if(font[ind++]!=0xC8){
		return 2; // Error signature, format error
	}

	if(font[ind++]!=0x05){
		return 3; // Error structure length
	}

	ind++; // Font height

	chr_count  = font[ind++];
	chr_count |= ((uint16_t)font[ind++])<<8;

	uint16_t cmap_start=ind;
	uint16_t last_offset=cmap_start+chr_count*6; // Start of the character data
	uint16_t chr_offset;
	uint16_t offset_ind;

	#define CHR_UTF32  4  // Character utf32 code (4 bytes)
	#define CHR_OFFSET 2  // Character offset (2 bytes)

	for(uint16_t i=0;i<chr_count;i++){
		offset_ind=cmap_start+i*(CHR_UTF32+CHR_OFFSET) + CHR_UTF32; // Index of character offset data
		chr_offset=font[offset_ind++]; 				    // Offset address LSB byte
		chr_offset |=((uint16_t)font[offset_ind++])<<8; // Offset address MSB byte

		if(last_offset>chr_offset){
			return 4; // Offset address error
		}

		last_offset=chr_offset;
	}

	return 0; // Success
}



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

int16_t LFC_Print_Utf32_Chr(PRINT_FORM *print_form,uint32_t chr,int16_t pos_x,int16_t pos_y){

	// Check null pointer
	if(print_form==NULL){
		return 0;
	}
	if(print_form->display_context==NULL){
		return 0;
	}

	uint8_t str[7];


	// Convert utf-32 to utf-8
	if(LFC_Utf32_To_Utf8(chr,str)==0){
		return 0;
	}

	// If set boundary box draw it and show string
	if(print_form->config & LFC_BOUNDING_BOX){
		LFC_RECT rect;
		LFC_Str_Rect(print_form,str,pos_x,pos_y,&rect);

		uint8_t fill;
		if(print_form->config & LFC_INVERT){
			fill=1;
		}else{
			fill=0;
		}

		LFC_Draw_Rect(print_form->display_context,&rect, fill,0);
		return _LFC_Print(print_form,str, pos_x, pos_y)+ 2*print_form->padding;
	}

	// Show string Without boundary box
	return _LFC_Print(print_form,str, pos_x, pos_y);
}





uint8_t LFC_Utf32_Char_Rect(PRINT_FORM *print_form, const uint32_t chr,int16_t x,int16_t y,LFC_RECT *rect){
	// Check null pointer
	if(print_form==NULL){
		return 0;
	}
	if(print_form->display_context==NULL){
		return 0;
	}

	uint8_t str[7];

	// Convert utf-32 to utf-8
	if(LFC_Utf32_To_Utf8(chr,str)==0){
		return 0;
	}

	return LFC_Str_Rect(print_form, str, x, y,rect);

}




