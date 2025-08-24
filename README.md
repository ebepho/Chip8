# CHIP-8 Emulator with ImGui Debugger

This project is a CHIP-8 emulator with a comprehensive debugging interface built using ImGui. The debugger provides real-time visualization of the CPU state, memory, registers, and allows interactive debugging similar to professional development tools.

## Features

### Core Emulator
- Complete CHIP-8 CPU implementation
- 4KB memory with font data
- 64x32 monochrome display
- 16-key hexadecimal keypad
- Sound and delay timers
- Stack for subroutines

### Debug Interface
- **CPU State Window**: Shows program counter, stack pointer, index register, current instruction, and decoded instruction
- **Register Window**: Displays all 16 V registers in a convenient grid
- **Memory Window**: Hex editor-style memory viewer with PC highlighting
- **Controls Window**: Reset, pause/resume, step-through, and speed control
- **Keyboard Window**: Visual representation of the CHIP-8 keypad with real-time state

## Building

### Prerequisites
- CMake 3.15+
- SDL2 development libraries
- SDL2_image development libraries
- C++17 compatible compiler

### Build Instructions
```bash
mkdir build
cd build
cmake ..
make
```

### Running
```bash
./chip8 <ROM_file>
```

## Controls

### CHIP-8 Keypad Mapping
```
CHIP-8 Key    Keyboard Key
1 2 3 C  ->   1 2 3 4
4 5 6 D  ->   Q W E R  
7 8 9 E  ->   A S D F
A 0 B F  ->   Z X C V
```

## Architecture

The project is organized into several components:

- `src/chip8.cpp` - Core CHIP-8 CPU implementation
- `src/instructions.cpp` - CHIP-8 instruction set implementation
- `UI/graphics.cpp` - SDL2 graphics handling
- `UI/debugger.cpp` - ImGui debugging interface
- `include/` - Header files

## Technical Details

- **Display**: 64x32 pixels, black and white
- **Memory**: 4KB (4096 bytes)
- **Registers**: 16 general-purpose 8-bit registers (V0-VF)
- **Stack**: 16 levels of 16-bit values
- **Timers**: 60Hz delay and sound timers
- **Instruction Set**: Complete CHIP-8 instruction set implementation

The debugger uses ImGui for the interface, providing a modern, responsive debugging experience that runs alongside the emulator in real-time.
