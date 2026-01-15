/*
 * device_pane.c
 *
 *  Created on: Jan 15, 2026
 *      Author: nebula
 */


// Include font library
#include "../lfc_font_lib/lfc_font.h"

// SSD1306 Oled display driver
#include "../ssd1306_driver/ssd1306.h"

// Include C8 fonts
#include "../fonts/C8_fonts.h"





void Print_Battery_Level(PRINT_FORM *battery_level_form,uint8_t level_percent){

	/*
	  	  --Battery levels--
		f244 # battery_emty
		f243 # battery_qurter
		f242 # battery_half
		f241 # battery_three_qurter
		f240 # battery_full
	 */

	uint32_t battery_icons[5]={0xf244,0xf243,0xf242,0xf241,0xf240};
	uint8_t icon_index;
	int16_t pos_x;
	int16_t pos_y;

	uint32_t current_icon;

	LFC_RECT battery_rect;

	if(level_percent>100){
		level_percent=100;
	}

	icon_index=(level_percent/20)%5;

	current_icon = battery_icons[icon_index];

	LFC_Utf32_Char_Rect(battery_level_form,current_icon,0,0,&battery_rect);

	DISPLAY_CONTEXT * display = battery_level_form->display_context;

	// Top right position
	pos_x=display->width  - battery_rect.width;
	pos_y=display->height - battery_rect.height;

	LFC_Print_Utf32_Chr(battery_level_form,current_icon, pos_x, pos_y);


	return;
}



void Font_Awesome_Example(DISPLAY_CONTEXT * main_display){

	// Create print form
	PRINT_FORM print_form;

	// Set displayer
	print_form.display_context=main_display;

	// Set font (it defined in fonts/C8_fonts.h)
	print_form.font=Font_Awesome_Free_Solid_16;

	// Disable all extra properties
	print_form.config=LFC_SPACING;

	// Extra space between characters
	print_form.spacing=0;

	// Padding text
	print_form.padding=0;

	uint32_t counter=0;

	/*

	f0f3 # bell
	f1eb # wifi
   */

	/*

	--Big icons 40px--
	f21e # heart_pulse
	f015 # home
	f2c2 # id_card
	e55c # plug_circle
	f7b9 # radiation
	f2cc # shower
	f86d # icons
	f52f # gas_pump
	f084 # key
	f479 # medikal_kid
	f130 # microphone
	e598 # wheat_awn
	f2ed # trash_can

	*/

	uint32_t big_icons[13]={0xf21e,0xf015,0xf2c2,0xe55c,0xf7b9,0xf2cc,0xf86d,0xf52f,0xf084,0xf479,0xf130,0xe598,0xf2ed};
	// Create print form
	PRINT_FORM big_icons_form;
	LFC_Copy_Print_Form(&big_icons_form,&print_form);
	big_icons_form.font=Font_Awesome_Free_Solid_32;
	big_icons_form.spacing=10;
	big_icons_form.padding=6;



	uint32_t bell_icon=0xf0f3;
	uint32_t wifi_icon=0xf1eb;

	int16_t caret_x;

	// Set x position to right edge of the screen
	int16_t pos_x=main_display->width;
	int16_t pos_y=8;

	while(1){

		// Clear display
		SSD1306_Clear();

		print_form.spacing=4;

		caret_x=LFC_Print_Utf32_Chr(&print_form,wifi_icon, 0, 50);

		caret_x=LFC_Print_Utf32_Chr(&print_form,bell_icon, caret_x, 50);

		print_form.spacing=0;
		Print_Battery_Level(&print_form,(uint8_t)((counter>>3)%100));

		// Print big icons
		caret_x=pos_x;
		for(uint8_t i=0;i<13;i++){
			// Print icon and return next caret position for next icon
			caret_x=LFC_Print_Utf32_Chr(&big_icons_form,big_icons[i], caret_x, pos_y);
		}

		pos_x--;
		if(caret_x<0){
			pos_x=main_display->width;
		}

		// Flush display buffer
		SSD1306_Flush();

		counter++;

	}

}





