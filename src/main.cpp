#include <chrono>
#include <iostream>
#include <chrono>
#include <thread>
#include "chip8.h"
#include "graphics.h"
#include "const.h"

int main(int argc, char* argv[])
{
	Chip8 chip8;

	// Debugger handles all rendering and SDL management
	Graphics graphics;
	
	// Parse command line arguments (ROM is now optional)
	int scale = 10;
	float cycleDelay = 1.4f; // milliseconds per cycle (700 instructions/second)
	std::string romPath;
	bool romLoaded = false;
	
	if (argc >= 2) {

		// ROM file specified
		romPath = argv[1];
		romLoaded = true;

		// Optional scale and cycle delay
		if (argc >= 4) {
			scale = std::stoi(argv[2]);
			cycleDelay = std::stof(argv[3]);
		}
	} else {
		// No ROM specified - will show ROM selector
		std::cout << "CHIP-8 Emulator with Debugger" << std::endl;
		std::cout << "Usage: " << argv[0] << " [ROM file] [cycleDelay]" << std::endl;
		std::cout << "  ROM file: CHIP-8 ROM to load (optional - will show ROM selector if not provided)" << std::endl;
		std::cout << "  scale: Display scale factor (default: 10)" << std::endl;
		std::cout << "  cycleDelay: Milliseconds per CPU cycle (default: 1.4)" << std::endl;
		std::cout << "Starting without ROM - use the ROM selector to load a game..." << std::endl;
	}

	// Create a larger window to accommodate the organized debugger UI
	if (!graphics.Init(1200, 800)) {
		std::cout << "Failed to initialize debugger." << std::endl;
		return -1;
	}

	// Set up ROM directory for the selector (assuming executable is in build/ directory)
	graphics.SetRomsDirectory("../roms");

	// Load ROM if one was specified
	if (romLoaded) {
		chip8.LoadROM(romPath.c_str());
		graphics.SetRomPath(romPath);
	}

	// ROM selector is now integrated into the Controls window and will be visible automatically
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	auto lastTimerTime = std::chrono::high_resolution_clock::now();
	
	// Timer runs at 60Hz = 16.67ms per timer tick
	const float timerInterval = 1000.0f / 60.0f; // ~16.67ms
	
	bool quit = false;
	SDL_Event event;

	while (!quit)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();

		// Handle SDL events
		while (SDL_PollEvent(&event))
		{
			// Let ImGui process the event first <- UI stuff
			graphics.ProcessEvent(&event);
			
			// Handle CHIP-8 keyboard input and check for quit
			if (!graphics.HandleInput(&event, chip8)) {
				quit = true;
			}
		}
		
		// Check for ROM load request
		if (graphics.IsRomLoadRequested()) {
			// Load the selected ROM
			std::string newRomPath = graphics.GetSelectedRomPath();

			// Only load if a valid path is provided
			if (!newRomPath.empty()) {
				chip8 = Chip8(); // Reset the system
				chip8.LoadROM(newRomPath.c_str());
				graphics.SetRomPath(newRomPath);
				romPath = newRomPath;
				romLoaded = true;
				std::cout << "Loaded ROM: " << newRomPath << std::endl;
			}
			graphics.RomLoadHandled();
		}
		
		// Check for reset request
		if (graphics.ShouldReset()) {

			// Reset the CHIP-8 system
			chip8 = Chip8();

			// Reload the current ROM if one is loaded
			if (romLoaded && !romPath.empty()) {
				chip8.LoadROM(romPath.c_str());
			}

			graphics.ResetHandled();
			continue; // Skip this frame to let reset complete
		}
		
		// Handle CPU cycles (only if ROM is loaded)
		if (romLoaded) {
			if (graphics.IsStepMode()) {
				chip8.Cycle();
				graphics.StepHandled();
			} 
			
			else if (!graphics.IsPaused()) {
				float cpuDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
				if (cpuDt > cycleDelay) {
					chip8.Cycle();
					lastCycleTime = currentTime;
				}
			}
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
		
		// Render everything in one call
		graphics.RenderFrame(chip8);
	}

	// Clean up
	graphics.Shutdown();

	return 0;
}