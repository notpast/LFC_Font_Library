# LFC Font Library  


LFC is an ultra-compact, memory-efficient bitmap font library designed specifically for resource-constrained embedded systems. With font files as small as 3-4KB and support for multiple displays, it provides professional typography capabilities for systems with less than 64KB of memory.



## Key Features

  ### Ultra-Compact Memory Footprint
  - Tiny font files: ~4KB for code space, ~3KB for 24px ASCII fonts
  - Zero heap usage: Completely malloc-free design
  - Minimal overhead: < 100 bytes RAM usage per display context
  - Optimized for: Systems with 64KB or less total memory  


 ### Full Unicode Support  
  - Complete UTF-8 encoding/decoding
  - Direct UTF-32 character rendering
  - Multi-language text capabilities
  - Efficient Unicode code point lookup


### Advanced Display Management  
  - Multiple display support: Drive different screens simultaneously
  - Dynamic screen rotation: 0°, 90°, 180°, 270° orientations
  - Display-independent rendering: Same API for all display types
  - Flexible pixel callbacks: Custom display driver integration


###  Professional Text Rendering  

  - Configurable character spacing and padding
  - Text inversion (black/white mode)
  - Bounding box visualization
  - Rectangle drawing (filled or outline)
  - Multi-display synchronized operations



## LFC Font Library - Dependencies
### 1. Display Driver / Pixel Writing Function

The library requires a display-specific driver or a pixel writing function compatible with your embedded system's display hardware.

**What you need to implement:**
- A hardware-specific function that writes individual pixels to your display
- Display resolution

### 2. C8 Bitmap Font (Converted via LFC Font Converter)
The library requires C8 format bitmap fonts that must be converted using the LFC Font Converter tool.


# LFC Font Library - Installation and Usage Guide
## How to Install

### Step 1: Add Library Files to Your Project

First, copy the lfc_font_lib folder and add it to your project. The folder contains two files. Alternatively, you can add only these two files directly to your project if you prefer.


### Step 2: Include the Header File

To use the LFC font library, include the lfc_font.h file in your desired source file using the #include directive.

```c

	// Include LFC font library
	#include "lfc_font_lib/lfc_font.h"

```



### Step 3: Define Display Context

Define a variable using the DISPLAY_CONTEXT structure defined in lfc_font.h and store your display's specifications within this structure.

```c


	DISPLAY_CONTEXT device_display;

	// Set display properties (for ssd1306)
	device_display.width          = 128; // Display width
	device_display.height         = 64;  // Display height
	device_display.rotation       = 0;	 // Screen rotation (0-3), 0: 0, 1:90, 2:180, 3:270 degree
	device_display.set_pixel_func = SSD1306_Set_Pixel; // Pixel write function, it defined in your display library



```


### Step 4: Configure Print Form

The library uses the PRINT_FORM structure to write text or icons to the display. Define a variable using this structure and configure the C8 font array and other text display properties.

```c


	// Create print form
	PRINT_FORM print_form;

	// Set displayer
	print_form.display_context = &main_display;

	// Set font (it defined in fonts/C8_fonts.h Create by LFC Font Converter)
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
	int16_t pos_x=10;

	// Text position: Y coordinate
	int16_t pos_y=10;


```


### Step 5: Display Text or Icons

You can display any UTF-8 string or icon on the screen using the LFC_Print_Str function.

```c

  uint8_t str[]="Hello World";
	// Print text
	LFC_Print(&print_form,str,pos_x,pos_y);

```


