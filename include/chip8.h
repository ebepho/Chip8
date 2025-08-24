#pragma once
#include <cstdint>
#include <fstream>
#include "const.h"

class Chip8
{
public:
	// Attributes ******************************************************************
	// Memory map (4096 bytes):
	// 0x000-0x1FF: Reserved. Store fontset somewhere in here (050–09F)
	// 0x200-0xFFF: Instructions from the ROM 
	uint8_t memory[MEMORY_SIZE]{};

	// Monochrome display (black & white), 64x32 pixels
	// Each pixel is either on or off
	// (originally updates at 60Hz, for simplicity we'll redraw it when executing instructions that change the display)
	uint32_t display[DISPLAY_SIZE]{};

	// Program counter, current instruction address
	uint16_t pc;

	// index register, point at location in memory
	uint16_t index;

	// Stack and stack pointer
	uint16_t stack[STACK_SIZE]{};
	uint8_t sp;

	// Timers -  as long as their value is above 0, they should be decremented by one, 60 times per second (ie. at 60 Hz)
	// independent of the speed of the fetch/decode/execute loop
	// Delay timer: decremented at a rate of 60Hz until it reaches 0
	// Sound timer: decremented at a rate of 60Hz until it reaches 0. When non-zero, the system should make a beeping sound
	uint8_t delayTimer;
	uint8_t soundTimer;

	// 16 registers, from V0 to VF. VF is also used as a flag by some instructions
	uint8_t registers[REGISTER_COUNT]{};
	uint8_t * VF = &registers[15];

	// Keypad mapping:
	// 	Keypad       Keyboard
	// +-+-+-+-+    +-+-+-+-+
	// |1|2|3|C|    |1|2|3|4|
	// +-+-+-+-+    +-+-+-+-+
	// |4|5|6|D|    |Q|W|E|R|
	// +-+-+-+-+ => +-+-+-+-+
	// |7|8|9|E|    |A|S|D|F|
	// +-+-+-+-+    +-+-+-+-+
	// |A|0|B|F|    |Z|X|C|V|
	// +-+-+-+-+    +-+-+-+-+
	uint8_t keypad[16]{};

	// Methods *******************************************************************
	// Setup
	Chip8();
	void LoadROM(char const* filename);

	// CPU Loop
	void Cycle();
	void DecodeAndExecute(uint16_t opcode);
	
    // Instructions **************************************************************
	// Clear screen
    void OP_00E0();

	// Jump
	void OP_1NNN(uint16_t address);

	// Set register VX
	void OP_6XNN(uint8_t Vx, uint8_t byte);

	// Add value to register VX
	void OP_7XNN(uint8_t Vx, uint8_t byte);

	// Set index register
	void OP_ANNN(uint16_t address);

	// Display/Draw
	void OP_DXYN(uint8_t Vx, uint8_t Vy, uint8_t height);
};



