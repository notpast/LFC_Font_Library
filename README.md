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




# LFC Font Library - Installation and Usage Guide
## How to Install

### Step 1: Add Library Files to Your Project

First, copy the lfc_font_lib folder and add it to your project. The folder contains two files. Alternatively, you can add only these two files directly to your project if you prefer.

Example Code:


### Step 2: Include the Header File

To use the LFC font library, include the lfc_font.h file in your desired source file using the #include directive.

Example Code:



### Step 3: Define Display Context

Define a variable using the DISPLAY_CONTEXT structure defined in lfc_font.h and store your display's specifications within this structure.

Example Code:


### Step 4: Configure Print Form

The library uses the PRINT_FORM structure to write text or icons to the display. Define a variable using this structure and configure the C8 font array and other text display properties.

Example Code:


### Step 5: Display Text or Icons

You can display any UTF-8 string or icon on the screen using the LFC_Print_Str function.

Example Code:
