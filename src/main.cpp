#include <chrono>
#include <iostream>
#include <chrono>
#include <thread>
#include "chip8.h"
#include "graphics.h"
#include "debugger.h"
#include "const.h"

int main(int argc, char* argv[])
{
	Chip8 chip8;
	Graphics graphics;
	Debugger debugger;
	
	// Parse command line arguments (ROM is now optional)
	int scale = 10;
	float cycleDelay = 1.4f; // milliseconds per cycle (700 instructions/second)
	std::string romPath;
	bool romLoaded = false;
	
	if (argc >= 2) {
		// ROM file specified
		romPath = argv[1];
		romLoaded = true;
		scale = (argc >= 3) ? std::stoi(argv[2]) : 10;
		cycleDelay = (argc >= 4) ? std::stof(argv[3]) : 1.4f;
	} else {
		// No ROM specified - will show ROM selector
		std::cout << "CHIP-8 Emulator with Debugger" << std::endl;
		std::cout << "Usage: " << argv[0] << " [ROM file] [scale] [cycleDelay]" << std::endl;
		std::cout << "  ROM file: CHIP-8 ROM to load (optional - will show ROM selector if not provided)" << std::endl;
		std::cout << "  scale: Display scale factor (default: 10)" << std::endl;
		std::cout << "  cycleDelay: Milliseconds per CPU cycle (default: 1.4)" << std::endl;
		std::cout << "Starting without ROM - use the ROM selector to load a game..." << std::endl;
	}

	// Create a larger window to accommodate the organized debugger UI
	if (!graphics.Init("Chip8 Emulator with Debugger", 1200, 800, DISPLAY_WIDTH, DISPLAY_HEIGHT)) {
		std::cout << "Failed to initialize graphics." << std::endl;
		return -1;
	}

	// Initialize the debugger
	if (!debugger.Init(graphics.GetWindow(), graphics.GetRenderer())) {
		std::cout << "Failed to initialize debugger." << std::endl;
		return -1;
	}

	// Set up ROM directory for the selector (assuming executable is in build/ directory)
	debugger.SetRomsDirectory("../roms");

	// Load ROM if one was specified
	if (romLoaded) {
		chip8.LoadROM(romPath.c_str());
		debugger.SetRomPath(romPath);
	}
	// ROM selector is now integrated into the Controls window and will be visible automatically

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
			// Let ImGui process the event first
			debugger.ProcessEvent(&event);
			
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			
			// Handle keyboard input for CHIP-8
			if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				bool keyPressed = (event.type == SDL_KEYDOWN);
				
				switch (event.key.keysym.sym) {
					case SDLK_1: chip8.keypad[0x1] = keyPressed; break;
					case SDLK_2: chip8.keypad[0x2] = keyPressed; break;
					case SDLK_3: chip8.keypad[0x3] = keyPressed; break;
					case SDLK_4: chip8.keypad[0xC] = keyPressed; break;
					case SDLK_q: chip8.keypad[0x4] = keyPressed; break;
					case SDLK_w: chip8.keypad[0x5] = keyPressed; break;
					case SDLK_e: chip8.keypad[0x6] = keyPressed; break;
					case SDLK_r: chip8.keypad[0xD] = keyPressed; break;
					case SDLK_a: chip8.keypad[0x7] = keyPressed; break;
					case SDLK_s: chip8.keypad[0x8] = keyPressed; break;
					case SDLK_d: chip8.keypad[0x9] = keyPressed; break;
					case SDLK_f: chip8.keypad[0xE] = keyPressed; break;
					case SDLK_z: chip8.keypad[0xA] = keyPressed; break;
					case SDLK_x: chip8.keypad[0x0] = keyPressed; break;
					case SDLK_c: chip8.keypad[0xB] = keyPressed; break;
					case SDLK_v: chip8.keypad[0xF] = keyPressed; break;
				}
			}
		}

		auto currentTime = std::chrono::high_resolution_clock::now();
		
		// Check for ROM load request
		if (debugger.IsRomLoadRequested()) {
			std::string newRomPath = debugger.GetSelectedRomPath();
			if (!newRomPath.empty()) {
				chip8 = Chip8(); // Reset the system
				chip8.LoadROM(newRomPath.c_str());
				debugger.SetRomPath(newRomPath);
				romPath = newRomPath;
				romLoaded = true;
				std::cout << "Loaded ROM: " << newRomPath << std::endl;
			}
			debugger.RomLoadHandled();
		}
		
		// Check for reset request
		if (debugger.ShouldReset()) {
			// Reset the CHIP-8 system
			chip8 = Chip8();
			// Reload the current ROM if one is loaded
			if (romLoaded && !romPath.empty()) {
				chip8.LoadROM(romPath.c_str());
			}
			debugger.ResetHandled();
			continue; // Skip this frame to let reset complete
		}
		
		// Handle CPU cycles (only if not paused and ROM is loaded)
		if (!debugger.IsPaused() && romLoaded) {
			float cpuDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
			if (cpuDt > cycleDelay) {
				lastCycleTime = currentTime;
				
				// Execute one instruction
				chip8.Cycle();
				
				// If in step mode, pause after executing one instruction
				if (debugger.IsStepMode()) {
					debugger.StepHandled(); // This will set step mode to false
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
		
		// Render everything
		graphics.Clear();
		
		// Render ImGui debugger interface (includes display now)
		debugger.NewFrame();
		debugger.Render(chip8);
		debugger.EndFrame();
		
		// Present the final frame
		graphics.Present();
	}

	// Clean up
	debugger.Shutdown();

	return 0;
}