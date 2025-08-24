#include <chrono>
#include <iostream>
#include <chrono>
#include <thread>
#include "chip8.h"
#include "graphics.h"
#include "const.h"

int main(int argc, char* argv[])
{
	if(argc < 2 || argc > 4)
	{
		std::cout << "Usage: " << argv[0] << " <ROM file> [scale] [cycleDelay]" << std::endl;
		std::cout << "  scale: Display scale factor (default: 10)" << std::endl;
		std::cout << "  cycleDelay: Milliseconds per CPU cycle (default: 1.4)" << std::endl;
		return 1;
	}

	Chip8 chip8;
	Graphics graphics;
	
	// Parse command line arguments
	int scale = (argc >= 3) ? std::stoi(argv[2]) : 10;

	float cycleDelay = (argc >= 4) ? std::stof(argv[3]) : 1.4f; // milliseconds per cycle (700 instructions/second)

	if (!graphics.Init("Chip8 Emulator", DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale, DISPLAY_WIDTH, DISPLAY_HEIGHT)) {
		std::cout << "Failed to initialize graphics." << std::endl;
		return -1;
	}

	chip8.LoadROM(argv[1]);
	int videoPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH; // Assuming 32-bit pixels

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	auto lastTimerTime = std::chrono::high_resolution_clock::now();
	
	// Timer runs at 60Hz = 16.67ms per timer tick
	const float timerInterval = 1000.0f / 60.0f; // ~16.67ms
	
	bool quit = false;
	SDL_Event event;

	while (!quit)
	{
		// Handle SDL events
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		auto currentTime = std::chrono::high_resolution_clock::now();
		
		// Handle CPU cycles
		float cpuDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		if (cpuDt > cycleDelay)
		{
			lastCycleTime = currentTime;
			
			chip8.Cycle();

			// Update graphics
			graphics.Update(chip8.display, DISPLAY_WIDTH, DISPLAY_HEIGHT, videoPitch);
		}
		
		// Handle timers at 60Hz (independent of CPU speed)
		float timerDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTimerTime).count();
		if (timerDt >= timerInterval)
		{
			lastTimerTime = currentTime;
			
			// Decrement timers at 60Hz
			if (chip8.delayTimer > 0) 
			{
				chip8.delayTimer--;
			}
			if (chip8.soundTimer > 0) 
			{
				chip8.soundTimer--;
				// TODO: Implement beeping sound when soundTimer > 0
			}
		}
	}

	return 0;
}