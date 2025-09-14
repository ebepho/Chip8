# CHIP-8 Emulator with ImGui Debugger

This project is a CHIP-8 emulator with a comprehensive debugging interface built using ImGui. The debugger provides real-time visualization of the CPU state, memory, registers, and allows interactive debugging similar to professional development tools.

<img width="2386" height="1634" alt="image" src="https://github.com/user-attachments/assets/d1247955-6dc1-4764-a634-13048ef12e54" />


## Features

### Core Emulator
- Complete CHIP-8 CPU implementation
- 4KB memory with font data
- 64x32 monochrome display
- 16-key hexadecimal keypad
- Sound and delay timers
- Stack for subroutines

### Debug Interface
- **ROM Selector**: Interactive file browser for loading ROMs from the `roms/` directory
- **CPU State Window**: Shows program counter, stack pointer, index register, current instruction, and decoded instruction with instruction history
- **Register Window**: Displays all 16 V registers in a convenient grid layout with real-time updates
- **Memory Window**: Hex editor-style memory viewer with PC highlighting and navigation controls
- **Stack Window**: Real-time stack visualization showing actual values and stack pointer position
- **Controls Window**: Functional reset, pause/resume, single-step execution, and ROM loading controls
- **Keyboard Window**: Interactive CHIP-8 keypad with press/release visual feedback and proper key mapping
- **Display Window**: Pixel-perfect CHIP-8 display rendering with proper black and white output
- **Organized Layout**: Professional window arrangement that fits perfectly on screen

## Building

### Prerequisites
- CMake 3.15+
- SDL2 development libraries
- SDL2_image development libraries  
- C++17 compatible compiler
- Git (for automatic ImGui dependency fetching)

### Build Instructions
```bash
# Clone the repository
git clone <repository-url>
cd Chip8

# Create build directory and build
mkdir build
cd build
cmake ..
make
```

**Note**: ImGui is automatically downloaded and built as part of the CMake configuration using FetchContent.

### Running
```bash
# Start with ROM selector (recommended)
./chip8

# Or load a specific ROM directly
./chip8 <ROM_file>

# With custom settings
./chip8 [ROM_file] [cycleDelay]
```

**Parameters:**
- `ROM_file`: Path to CHIP-8 ROM (optional - ROM selector will appear if not provided)
- `scale`: Display scale factor (default: 10)
- `cycleDelay`: Milliseconds per CPU cycle for emulation speed (default: 1.4, ~700 instructions/s)

## Controls

### Debug Controls
- **Reset**: Completely resets the CHIP-8 system and reloads the current ROM
- **Pause/Resume**: Toggle emulation execution (shows current state)
- **Step**: Execute exactly one instruction when paused (for precise debugging)
- **Load ROM**: Opens ROM selector to browse and load ROMs from the `roms/` directory

### ROM Selection
- **Startup**: ROM selector appears automatically when starting without specifying a ROM
- **File Menu**: Access "Load ROM..." from the File menu at any time
- **Browse ROMs**: Visual list of all available ROMs in the `roms/` directory
- **Quick Loading**: Double-click or select and click "Load ROM" to start playing

### CHIP-8 Keypad Mapping
```
CHIP-8 Key    Keyboard Key
1 2 3 C  ->   1 2 3 4
4 5 6 D  ->   Q W E R  
7 8 9 E  ->   A S D F
A 0 B F  ->   Z X C V
```

### Debug Interface Navigation
- All windows are automatically positioned for optimal layout
- Click and drag window titles to reposition if needed
- Memory window supports scrolling and PC following
- Register values update in real-time during execution

## Architecture

The project is organized into several components:

- `src/chip8.cpp` - Core CHIP-8 CPU implementation
- `src/instructions.cpp` - CHIP-8 instruction set implementation  
- `src/main.cpp` - Main emulation loop with debugger integration
- `UI/graphics.cpp` - SDL2 graphics handling and ImGui rendering
- `include/chip8.h` - CHIP-8 system header with core definitions
- `include/const.h` - System constants and configuration
- `roms/` - Collection of CHIP-8 ROM files for testing

## Implementation Details

### ImGui Integration
- ImGui 1.91.5 with SDL2 renderer backend
- Automatic dependency management via CMake FetchContent
- Professional window layout system with perfect positioning
- Real-time rendering alongside emulation

### Graphics System
- Pixel-perfect CHIP-8 display rendering
- Proper black and white output (no color artifacts)
- SDL2 texture handling with ABGR format
- Smooth integration with ImGui rendering pipeline

### Control System  
- State-based execution control (running/paused/stepping/reset)
- Integrated main loop that respects all debug states
- Proper ROM reloading on reset
- Thread-safe state management

## Technical Details
- **Display**: 64x32 pixels, black and white
- **Memory**: 4KB (4096 bytes) 
- **Registers**: 16 general-purpose 8-bit registers (V0-VF)
- **Stack**: 16 levels of 16-bit values with visual stack viewer
- **Timers**: 60Hz delay and sound timers
- **Instruction Set**: Complete CHIP-8 instruction set implementation
- **Debug Features**: Real-time instruction disassembly, memory visualization, register monitoring
- **UI Framework**: ImGui with SDL2 backend for cross-platform compatibility

The debugger uses ImGui for the interface, providing a modern, responsive debugging experience that runs alongside the emulator in real-time. The interface is designed to mimic professional debugging tools with organized windows and comprehensive system state visualization.
