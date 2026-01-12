/*
 * simple_text_print.c
 *
 *  Created on: Jan 8, 2026
 *      Author: nebula
 */



// Include font library
#include "../lfc_font_lib/lfc_font.h"

// SSD1306 Oled display driver
#include "../ssd1306_driver/ssd1306.h"

// Include C8 fonts
#include "../fonts/C8_fonts.h"




int16_t Horizontal_Center(PRINT_FORM * p_form,uint8_t *str){
	LFC_RECT rect;

	if(LFC_Str_Rect(p_form, str,0,0,&rect)){
		return 0; //Error
	}

	uint16_t screen_width;

	uint8_t rotation=p_form->display_context->rotation;

	// 0 and 180 degree
	if( (rotation%2)==0) {
		screen_width=p_form->display_context->width;
	}else{
		screen_width=p_form->display_context->height;
	}

	return (screen_width-rect.width)/2;
}


int16_t Vertical_Center(PRINT_FORM * p_form,uint8_t *str){
	LFC_RECT rect;

	if(LFC_Str_Rect(p_form, str,0,0,&rect)){
		return 0; //Error
	}

	uint16_t screen_height;

	uint8_t rotation=p_form->display_context->rotation;

	// 0 and 180 degree
	if( (rotation%2)==0) {
		screen_height=p_form->display_context->height;
	}else{
		screen_height=p_form->display_context->width;
	}

	return (screen_height-rect.height)/2;
}







void Simple_Print_Text(DISPLAY_CONTEXT * main_display){

	// Create print form
	PRINT_FORM print_form;

	// Set displayer
	print_form.display_context=main_display;

	// Set font (it defined in fonts/C8_fonts.h)
	print_form.font=Goldman_Regular_16;

	// Disable all extra properties
	print_form.config=LFC_DEFAULT_CONFIG;

	// Extra space between characters
	print_form.spacing=0;

	// Padding text
	print_form.padding=0;

	// Text
	uint8_t str[]="LFC Text";

	// Text position: X coordinate
	int16_t pos_x=20;

	// Text position: Y coordinate
	int16_t pos_y=30;

	// Clear display
	SSD1306_Clear();

	// Print text
	LFC_Print(&print_form,str,pos_x,pos_y);

	// Flush display buffer
	SSD1306_Flush();

}





void Text_With_Extra_Space(DISPLAY_CONTEXT * main_display){

	// Create print form
	PRINT_FORM print_form;

	// Set displayer
	print_form.display_context=main_display;

	// Set font (it defined in fonts/C8_fonts.h)
	print_form.font=Goldman_Regular_16;

	// Enable extra space
	print_form.config=LFC_SPACING;

	// Extra space between characters
	print_form.spacing=3;

	// Padding text
	print_form.padding=0;


	// Text
	uint8_t str[]="LFC Text";

	// Text position X coordinate
	int16_t pos_x=20;

	// Text position Y coordinate
	int16_t pos_y=30;


	// Clear display
	SSD1306_Clear();

	// Print text
	LFC_Print(&print_form,str,pos_x,pos_y);

	// Flush display buffer
	SSD1306_Flush();

}


void Text_With_Boundary_Box(DISPLAY_CONTEXT * main_display){

	// Create print form
	PRINT_FORM print_form;

	// Set displayer
	print_form.display_context=main_display;

	// Set font (it defined in fonts/C8_fonts.h)
	print_form.font=Goldman_Regular_16;

	// Enable boundary box
	print_form.config = LFC_BOUNDING_BOX;

	// Extra space between characters
	print_form.spacing = 0;

	// Padding text
	print_form.padding = 8;

	// Text
	uint8_t str[]="LFC Font";

	// Text position X coordinate
	int16_t pos_x=20;

	// Text position Y coordinate
	int16_t pos_y=30;

	// Clear display
	SSD1306_Clear();

	// Print text
	LFC_Print(&print_form,str,pos_x,pos_y);

	// Flush display buffer
	SSD1306_Flush();

}
