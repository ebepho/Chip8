#include "chip8.h"

// Clear screen
void Chip8::OP_00E0()
{
	for(int i = 0; i < DISPLAY_SIZE; ++i)
	{
		display[i] = 0x00000000; // Black pixels
	}
}

// Jump
void Chip8::OP_1NNN(uint16_t address)
{
	pc = address;
}

// Set register VX
void Chip8::OP_6XNN(uint8_t Vx, uint8_t byte){
	registers[Vx] = byte;
}

// Add value to register VX
void Chip8::OP_7XNN(uint8_t Vx, uint8_t byte){
	registers[Vx] += byte;
}

// Set index register
void Chip8::OP_ANNN(uint16_t address){
	index = address;
}

// Display/Draw
void Chip8::OP_DXYN(uint8_t Vx, uint8_t Vy, uint8_t height){
	// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
	// index register points to the location in memory where the sprite's data is stored.
	// All the pixels are XORed onto the existing screen.

	// Get coordinates from registers and wrap around if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % DISPLAY_WIDTH;
	uint8_t yPos = registers[Vy] % DISPLAY_HEIGHT;

	*VF = 0; // reset collision flag

	// For each row of the sprite
	for(unsigned int row = 0; row < height; ++row)
	{
		// Stop if we reach the bottom of the screen
		if((yPos + row) >= DISPLAY_HEIGHT) break;

		// Get the sprite byte for this row
		uint8_t spriteByte = memory[index + row];

		// For each bit in the sprite byte (8 pixels wide)
		for(unsigned int col = 0; col < 8; ++col)
		{
			// Stop if we reach the right edge of the screen
			if((xPos + col) >= DISPLAY_WIDTH) break;

			// Extract the pixel value (1 or 0) from the sprite byte
			uint8_t spritePixel = spriteByte & (0x80 >> col);
			
			// Calculate screen pixel position
			uint32_t* screenPixel = &display[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];

			// If sprite pixel is on (1)
			if(spritePixel)
			{
				// Check for collision (both sprite and screen pixels are on)
				if(*screenPixel == 0xFFFFFFFF)
				{
					*VF = 1; // Set collision flag
				}

				// XOR the pixel (toggle it)
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}