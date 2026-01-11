#include "ssd1306.h"

/*
 * @brief Custom delay function in milliseconds
 * @param ms: Delay duration in milliseconds
 * @note For STM32 @ 72Mhz
 */
static void delay_ms(uint32_t ms){
	volatile uint32_t j;
	for(uint32_t i=0;i<ms;i++){
		for(j=0;j<4000;j++);
	}
}

/*
 * @brief Send a command to SSD1306 display over I2C
 * @param cmd: Command byte to send
 * @return 0 if successful, error code if failed
 * @note Error codes:
 *       1: I2C START condition failed
 *       2: I2C address transmission failed
 *       3: Control byte transmission failed
 *       4: Command byte transmission failed
 */
uint8_t SSD1306_Send_Command(uint8_t cmd) {

	uint32_t timeout;
	uint32_t timeout_reload=20000; // Timeout counter value

		// Generate I2C START condition
		I2C_GenerateSTART(I2C_CON, ENABLE);
		timeout=timeout_reload;
		// Wait for master mode selected event with timeout
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_MODE_SELECT)){
			if(!timeout--){
				return 1; // Timeout error - START condition failed
			}
		}

		// Send SSD1306 I2C address in transmitter mode
		I2C_Send7bitAddress(I2C_CON, SSD1306_I2C_ADDRESS, I2C_Direction_Transmitter);
		timeout=timeout_reload;
		// Wait for transmitter mode selected event
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
			if(!timeout--){
				return 2; // Timeout error - address transmission failed
			}
		}

		// Send control byte (0x00 for command)
		I2C_SendData(I2C_CON, 0x00);
		timeout=timeout_reload;
		// Wait for control byte transmission complete
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
			if(!timeout--){
				return 3; // Timeout error - control byte failed
			}
		}

		// Send the actual command byte
		I2C_SendData(I2C_CON, cmd);
		timeout=timeout_reload;
		// Wait for command byte transmission complete
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
			if(!timeout--){
				return 4; // Timeout error - command transmission failed
			}
		}

		// Generate I2C STOP condition
		I2C_GenerateSTOP(I2C_CON, ENABLE);

		return 0; // Success
}

/*
 * @brief Flush display buffer to SSD1306 RAM (update display)
 * @return 0 if successful, error code if failed
 * @note Error codes same as SSD1306_Send_Command
 *       This function sends the entire display buffer to the OLED
 */
uint8_t SSD1306_Flush() {

	uint32_t timeout;
	uint32_t timeout_reload=20000;

	// Set column address range (0-127)
	SSD1306_Send_Command(0x21); // Column address command
	SSD1306_Send_Command(0x00); // Start column
	SSD1306_Send_Command(127);  // End column

	// Set page address range (0-7 for 64px height)
	SSD1306_Send_Command(0x22); // Page address command
	SSD1306_Send_Command(0x00); // Start page
	SSD1306_Send_Command(7);    // End page

		// Generate I2C START condition
		I2C_GenerateSTART(I2C_CON, ENABLE);
		timeout=timeout_reload;
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_MODE_SELECT)){
			if(!timeout--){
				return 1; // Timeout error
			}
		}

		// Send SSD1306 I2C address
		I2C_Send7bitAddress(I2C_CON, SSD1306_I2C_ADDRESS, I2C_Direction_Transmitter);
		timeout=timeout_reload;
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
			if(!timeout--){
				return 2; // Timeout error
			}
		}

		// Send data control byte (0x40 for data)
		I2C_SendData(I2C_CON, 0x40);
		timeout=timeout_reload;
		while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
			if(!timeout--){
				return 3; // Timeout error
			}
		}

		// Send entire display buffer (1024 bytes for 128x64 display)
		uint32_t len=(128*64)/8; // Calculate buffer size in bytes
		for(uint32_t i=0;i<len;i++){
			I2C_SendData(I2C_CON, display_buffer[i]);
			timeout=timeout_reload;
			while (!I2C_CheckEvent(I2C_CON, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
				if(!timeout--){
					return 4; // Timeout error
				}
			}
		}

		// Generate I2C STOP condition
		I2C_GenerateSTOP(I2C_CON, ENABLE);
		return 0; // Success
}

/*
 * @brief Control display power state
 * @param power_mode: 1 = display ON, 0 = display OFF
 */
void SSD1306_Power(uint8_t power_mode) {
	if (power_mode == 1) {
		 SSD1306_Send_Command(0xAF); // Display ON command
	} else if (power_mode == 0) {
		 SSD1306_Send_Command(0xAE); // Display OFF command
	}
	return;
}

/*
 * @brief Clear display buffer (set all pixels to OFF)
 * @note This only clears the software buffer, call SSD1306_Flush() to update display
 */
void SSD1306_Clear() {
	uint32_t i;
	// Clear entire display buffer (set all bytes to 0x00)
	for (i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT) / 8; i++) {
		display_buffer[i] = 0x00;
	}
}

/*
 * @brief Initialize SSD1306 display
 * 		  This function sends the initialization sequence to configure the display
 * @return 0 if successful
 */
uint8_t SSD1306_Init()
{
	delay_ms(100); // Wait for display to power up

	// Display initialization sequence
	SSD1306_Send_Command(0xAE); // Display OFF
	SSD1306_Send_Command(0x00); // Set lower column address
	SSD1306_Send_Command(0x10); // Set higher column address
    SSD1306_Send_Command(0x40); // Set display start line to 0

    // Contrast settings
    SSD1306_Send_Command(0x81); SSD1306_Send_Command(0xCC); // Set contrast level

    // Display mapping and orientation
    SSD1306_Send_Command(0xA1); // Segment remap (horizontal flip)
    SSD1306_Send_Command(0xC8); // COM output scan direction (vertical flip)

    // Display mode settings
    SSD1306_Send_Command(0xA6); // Normal display (not inverted)
    SSD1306_Send_Command(0xA4); // Display follows RAM content

    // Multiplex ratio and display offset
    SSD1306_Send_Command(0xA8); SSD1306_Send_Command(0x3F); // Set multiplex ratio to 64
    SSD1306_Send_Command(0xD3); SSD1306_Send_Command(0x00); // Set display offset to 0

    // Timing and driving settings
    SSD1306_Send_Command(0xD5); SSD1306_Send_Command(0xF0); // Set display clock divide ratio/oscillator frequency
    SSD1306_Send_Command(0xD9); SSD1306_Send_Command(0xF1); // Set pre-charge period
    SSD1306_Send_Command(0xDA); SSD1306_Send_Command(0x12); // Set COM pins hardware configuration
    SSD1306_Send_Command(0xDB); SSD1306_Send_Command(0x40); // Set VCOMH deselect level

    // Memory addressing mode
    SSD1306_Send_Command(0x20); SSD1306_Send_Command(0x00); // Set horizontal addressing mode

    // Charge pump settings
    SSD1306_Send_Command(0x8D); SSD1306_Send_Command(0x14); // Enable charge pump

    // Turn display ON
    SSD1306_Send_Command(0xAF); // Display ON

    delay_ms(100); // Stabilization delay

    return 0; // Success
}

/*
 * @brief Set individual pixel state in display buffer
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 * @param state: 1 = pixel ON, 0 = pixel OFF
 */
void SSD1306_Set_Pixel(uint16_t x, uint16_t y, uint8_t state) {
	uint16_t ind;
	// Validate coordinates
	if (x > 127 || y > 63) {
		return; // Invalid coordinates, do nothing
	}
	// Calculate buffer index: (y/8) gives page number, *128 gives page offset, +x gives column
	ind = ((y / 8) * SCREEN_WIDTH) + x;

	if (state) {
		display_buffer[ind] |= 1 << (y & 7);
	} else {
		display_buffer[ind] &= ~(1 << (y & 7));
	}
}

