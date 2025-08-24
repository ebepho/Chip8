#include "chip8.h"
#include <fstream>
#include <iostream>

Chip8::Chip8()
{
    // Initialize PC
    pc = PC_START_ADDRESS;

	// Load font data into memory
	for (unsigned int i = 0; i < FONT_SIZE; ++i)
	{
		memory[FONT_START_ADDRESS + i] = font[i];
	}
}

// Load Rom -> https://austinmorlan.com/posts/chip8_emulator/
void Chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	// Make sure the file exists
	if (!file)
	{
		std::cout << "Failed to open ROM: " << filename << std::endl;
		exit(1);
	}

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[PC_START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

void Chip8::Cycle()
{
	// Fetch: the instruction from memory at the current PC
	// An instruction is two bytes, so you will need to read two successive bytes from memory and combine them into one 16-bit instruction.
	uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
	pc += 2;

	// Decode:  the instruction to find out what the emulator should do
	// Excute: the instruction and do what it tells you
	DecodeAndExecute(opcode);
}


void Chip8::DecodeAndExecute(uint16_t opcode)
{
	uint8_t  op  = (opcode & 0xF000) >> 12; // 1st nibble - Tells you what kind of instruction it is
	uint8_t  x   = (opcode & 0x0F00) >> 8;  // 2nd nibble - Used to look up one of the 16 registers (VX) from V0 through VF
	uint8_t  y   = (opcode & 0x00F0) >> 4;  // 3rd nibble - Used to look up one of the 16 registers (VY) from V0 through VF

	uint8_t  n   = opcode & 0x000F;   // lowest 4 bits. - Used as a 4-bit immediate value for some instructions
	uint8_t  nn  = opcode & 0x00FF;   // lowest 8 bits  - Used as an 8-bit immediate value for some instructions
	uint16_t nnn = opcode & 0x0FFF;   // lowest 12 bits - Used as a 12-bit address for some instructions

	switch (op)
	{
		case 0x0:
			OP_00E0();
			break;

		case 0x1: 
			OP_1NNN(nnn); 
			break;

		case 0x6:
			OP_6XNN(x, nn);
			break;

		case 0x7: 
			OP_7XNN(x, nn); 
			break;

		case 0xA:
			OP_ANNN(nnn); 
			break;
		
		case 0xD: 
			OP_DXYN(x, y, n);
			break;
	}
}