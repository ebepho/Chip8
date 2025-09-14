#include "graphics.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstdio>

Graphics::Graphics() : showRegisters(true), showMemory(true), showControls(true), showCPUState(true), showKeyboard(true), showDisassembly(true), showDisplay(true), window(nullptr), renderer(nullptr), displayTexture(nullptr), isPaused(false), isStep(false), isReset(false), romLoadRequested(false), selectedRomIndex(-1) {}

bool Graphics::Init(int width, int height)
{
    const char* title = "Chip8 Emulator";
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    // Create texture for CHIP-8 display with proper format
    displayTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!displayTexture) {
        std::cout << "Failed to create display texture: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    // Setup initial window layout
    SetupWindowLayout();
    
    return true;
}

void Graphics::SetupWindowLayout()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float windowWidth = static_cast<float>(w);
    float windowHeight = static_cast<float>(h);
    
    float menuBarHeight = ImGui::GetFrameHeight();
    
    // Calculate available space (subtract menu bar and padding)
    float availableHeight = windowHeight - menuBarHeight - 20.0f;
    float availableWidth = windowWidth - 20.0f;
    
    // Define precise layout - organized in a grid
    // Left column: 280px - CPU State (top) + Controls (bottom)
    // Center column: 540px - Display (top) + Memory (bottom) 
    // Right column: 320px - Registers + Keyboard + Disassembly
    
    const float leftWidth = 280.0f;
    const float centerWidth = 540.0f;
    const float rightWidth = 320.0f;
    const float padding = 10.0f;
    
    // Left Column Layout
    const float leftX = padding;
    const float cpuStateHeight = 220.0f;
    const float controlsHeight = availableHeight - cpuStateHeight - padding;
    
    // Center Column Layout  
    const float centerX = leftX + leftWidth + padding;
    const float displayHeight = 300.0f;
    const float memoryHeight = availableHeight - displayHeight - padding;
    
    // Right Column Layout
    const float rightX = centerX + centerWidth + padding;
    const float registersHeight = 180.0f;
    const float keyboardHeight = 180.0f;
    const float disassemblyHeight = availableHeight - registersHeight - keyboardHeight - (2 * padding);
    
    // Store positions for use in RenderOrganizedLayout
    // CPU State (top-left)
    cpuStatePos = ImVec2(leftX, menuBarHeight + padding);
    cpuStateSize = ImVec2(leftWidth, cpuStateHeight);
    
    // Controls (bottom-left)
    controlsPos = ImVec2(leftX, menuBarHeight + padding + cpuStateHeight + padding);
    controlsSize = ImVec2(leftWidth, controlsHeight);
    
    // Display (top-center)
    displayPos = ImVec2(centerX, menuBarHeight + padding);
    displaySize = ImVec2(centerWidth, displayHeight);
    
    // Memory (bottom-center)
    memoryPos = ImVec2(centerX, menuBarHeight + padding + displayHeight + padding);
    memorySize = ImVec2(centerWidth, memoryHeight);
    
    // Registers (top-right)
    registersPos = ImVec2(rightX, menuBarHeight + padding);
    registersSize = ImVec2(rightWidth, registersHeight);
    
    // Keyboard (middle-right)
    keyboardPos = ImVec2(rightX, menuBarHeight + padding + registersHeight + padding);
    keyboardSize = ImVec2(rightWidth, keyboardHeight);
    
    // Disassembly (bottom-right)
    disassemblyPos = ImVec2(rightX, menuBarHeight + padding + registersHeight + padding + keyboardHeight + padding);
    disassemblySize = ImVec2(rightWidth, disassemblyHeight);
}

void Graphics::RenderOrganizedLayout(Chip8& chip8)
{
    // CPU State Window (top-left) - Always set position and size
    if (showCPUState) {
        ImGui::SetNextWindowPos(cpuStatePos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(cpuStateSize, ImGuiCond_Always);
        RenderCPUState(chip8);
    }
    
    // Controls Window (bottom-left)
    if (showControls) {
        ImGui::SetNextWindowPos(controlsPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(controlsSize, ImGuiCond_Always);
        RenderControls(chip8);
    }
    
    // Display Window (top-center)
    if (showDisplay) {
        ImGui::SetNextWindowPos(displayPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(displaySize, ImGuiCond_Always);
        RenderDisplay(chip8);
    }
    
    // Memory Window (bottom-center)
    if (showMemory) {
        ImGui::SetNextWindowPos(memoryPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(memorySize, ImGuiCond_Always);
        RenderMemory(chip8);
    }
    
    // Registers Window (top-right)
    if (showRegisters) {
        ImGui::SetNextWindowPos(registersPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(registersSize, ImGuiCond_Always);
        RenderRegisters(chip8);
    }
    
    // Keyboard Window (middle-right)
    if (showKeyboard) {
        ImGui::SetNextWindowPos(keyboardPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(keyboardSize, ImGuiCond_Always);
        RenderKeyboard(chip8);
    }
    
    // Disassembly Window (bottom-right)
    if (showDisassembly) {
        ImGui::SetNextWindowPos(disassemblyPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(disassemblySize, ImGuiCond_Always);
        RenderDisassembly(chip8);
    }
}

void Graphics::ProcessEvent(SDL_Event* event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

bool Graphics::HandleInput(SDL_Event* event, Chip8& chip8)
{
    if (event->type == SDL_QUIT) {
        return false; // Signal to quit
    }
    
    // Handle keyboard input for CHIP-8
    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
        bool keyPressed = (event->type == SDL_KEYDOWN);
        
        switch (event->key.keysym.sym) {
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
    
    return true; // Continue running
}

void Graphics::RenderCPUState(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - CPU State", &showCPUState, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    // Get current instruction
    uint16_t instruction = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
    
    // CPU State section
    ImGui::SeparatorText("CPU Registers");
    
    ImGui::Text("PC:"); ImGui::SameLine(80); ImGui::Text("0x%04X", chip8.pc);
    ImGui::Text("SP:"); ImGui::SameLine(80); ImGui::Text("%d", chip8.sp);
    ImGui::Text("I:"); ImGui::SameLine(80); ImGui::Text("0x%04X", chip8.index);
    
    ImGui::SeparatorText("Current Instruction");
    
    ImGui::Text("Opcode:"); ImGui::SameLine(80); 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
    ImGui::Text("0x%04X", instruction);
    ImGui::PopStyleColor();
    
    // Decode instruction for display
    std::string decoded = DecodeInstruction(instruction);
    ImGui::Text("ASM:"); ImGui::SameLine(80); 
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    ImGui::Text("%s", decoded.c_str());
    ImGui::PopStyleColor();
    
    ImGui::SeparatorText("Timers");
    ImGui::Text("Delay:"); ImGui::SameLine(80); ImGui::Text("%d", chip8.delayTimer);
    ImGui::Text("Sound:"); ImGui::SameLine(80); ImGui::Text("%d", chip8.soundTimer);
    
    // Stack display
    ImGui::SeparatorText("Stack");
    ImGui::Text("Stack Pointer: %d", chip8.sp);
    
    if (chip8.sp > 0) {
        ImGui::Text("Stack Contents:");
        // Show stack entries (most recent first)
        for (int i = chip8.sp - 1; i >= 0 && i >= chip8.sp - 8; i--) { // Show up to 8 entries
            if (i == chip8.sp - 1) {
                // Highlight the top of stack
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("  [%d]: 0x%04X (TOP)", i, chip8.stack[i]);
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("  [%d]: 0x%04X", i, chip8.stack[i]);
            }
        }
        if (chip8.sp > 8) {
            ImGui::Text("  ... (%d more entries)", chip8.sp - 8);
        }
    } else {
        ImGui::TextDisabled("Stack empty");
    }
    
    ImGui::End();
}

void Graphics::RenderRegisters(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Registers", &showRegisters, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::SeparatorText("General Purpose Registers");
    
    // Display registers in a neat table
    if (ImGui::BeginTable("RegisterTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        
        for (int i = 0; i < 16; i++) {
            ImGui::TableNextColumn();
            
            // Color VF register differently as it's the flag register
            if (i == 15) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
            }
            
            ImGui::Text("V%X", i);
            ImGui::SameLine();
            ImGui::Text("0x%02X", chip8.registers[i]);
            ImGui::SameLine();
            ImGui::TextDisabled("(%d)", chip8.registers[i]);
            
            if (i == 15) {
                ImGui::PopStyleColor();
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::End();
}

void Graphics::RenderMemory(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Memory", &showMemory, ImGuiWindowFlags_NoMove);
    
    static int memoryStart = 0x200; // Start at ROM area
    static bool followPC = false;
    
    ImGui::Checkbox("Follow PC", &followPC);
    ImGui::SameLine();
    
    if (followPC) {
        memoryStart = (chip8.pc / 16) * 16; // Align to 16-byte boundary
    }
    
    ImGui::SliderInt("Start Address", &memoryStart, 0, 4096-256, "0x%04X");
    
    ImGui::SeparatorText("Memory View");
    
    // Navigation buttons
    if (ImGui::Button("Font Data (0x50)")) memoryStart = 0x50;
    ImGui::SameLine();
    if (ImGui::Button("ROM Start (0x200)")) memoryStart = 0x200;
    ImGui::SameLine();
    if (ImGui::Button("Current PC")) memoryStart = (chip8.pc / 16) * 16;
    
    ImGui::Separator();
    
    // Display memory in hex editor style with fixed-width font
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default monospace if available
    
    // Header
    ImGui::Text("Address   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII");
    ImGui::Separator();
    
    ImGui::BeginChild("MemoryView", ImVec2(0, -1), true);
    
    for (int i = 0; i < 16; i++) {
        int addr = memoryStart + i * 16;
        if (addr >= 4096) break;
        
        // Address column
        ImGui::Text("0x%04X: ", addr);
        ImGui::SameLine();
        
        // Hex values
        for (int j = 0; j < 16 && (addr + j) < 4096; j++) {
            ImGui::SameLine();
            uint8_t byte = chip8.memory[addr + j];
            
            // Highlight PC location and next instruction
            if (addr + j == chip8.pc) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("%02X", byte);
                ImGui::PopStyleColor();
            } else if (addr + j == chip8.pc + 1) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
                ImGui::Text("%02X", byte);
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%02X", byte);
            }
        }
        
        ImGui::SameLine();
        ImGui::Text("  ");
        ImGui::SameLine();
        
        // ASCII representation
        for (int j = 0; j < 16 && (addr + j) < 4096; j++) {
            ImGui::SameLine();
            uint8_t byte = chip8.memory[addr + j];
            char c = (byte >= 32 && byte < 127) ? byte : '.';
            
            if (addr + j == chip8.pc || addr + j == chip8.pc + 1) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("%c", c);
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%c", c);
            }
        }
    }
    
    ImGui::EndChild();
    ImGui::PopFont();
    ImGui::End();
}

void Graphics::RenderControls(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Controls", &showControls, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::SeparatorText("Execution Control");
    
    // Main control buttons
    if (ImGui::Button("Reset", ImVec2(80, 30))) {
        isReset = true;
        instructionHistory.clear();
    }
    
    ImGui::SameLine();
    if (ImGui::Button(isPaused ? "Resume" : "Pause", ImVec2(80, 30))) {
        isPaused = !isPaused;
        isStep = false;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Step", ImVec2(80, 30))) {
        isStep = true;
        isPaused = true;
    }
    
    // Status indicators
    ImGui::SeparatorText("Status");
    if (isPaused) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("PAUSED");
        ImGui::PopStyleColor();
    } else if (isStep) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::Text("STEP MODE");
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text("RUNNING");
        ImGui::PopStyleColor();
    }
    
    ImGui::SeparatorText("Emulation Settings");
    
    ImGui::Text("Speed:");
    static float emulationSpeed = 1.0f;
    ImGui::SliderFloat("##Speed", &emulationSpeed, 0.1f, 10.0f, "%.1fx");
    
    ImGui::Text("Display Scale:");
    static int displayScale = 10;
    ImGui::SliderInt("##Scale", &displayScale, 1, 20);
    
    ImGui::SeparatorText("ROM Info");
    ImGui::TextWrapped("Current ROM:");
    if (!currentRomPath.empty()) {
        // Extract just the filename from the path
        size_t lastSlash = currentRomPath.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) ? 
                              currentRomPath.substr(lastSlash + 1) : currentRomPath;
        ImGui::TextWrapped("%s", filename.c_str());
    } else {
        ImGui::TextDisabled("No ROM loaded");
    }
    
    ImGui::SeparatorText("ROM Selection");
    
    // ROM list
    if (availableRoms.empty()) {
        ImGui::TextDisabled("No ROMs found");
        if (ImGui::Button("Scan ROMs", ImVec2(-1, 0))) {
            ScanForRoms();
        }
    } else {
        ImGui::Text("Available ROMs:");
        
        // Create a combo box with all available ROMs
        std::string previewValue = (selectedRomIndex >= 0 && selectedRomIndex < availableRoms.size()) 
                                  ? availableRoms[selectedRomIndex] 
                                  : "Select a ROM...";
                                  
        if (ImGui::BeginCombo("##romselect", previewValue.c_str())) {
            for (int i = 0; i < availableRoms.size(); i++) {
                bool isSelected = (selectedRomIndex == i);
                
                if (ImGui::Selectable(availableRoms[i].c_str(), isSelected)) {
                    selectedRomIndex = i;
                }
                
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        
        // Load ROM button
        bool canLoad = selectedRomIndex >= 0 && selectedRomIndex < availableRoms.size();
        
        if (!canLoad) {
            ImGui::BeginDisabled();
        }
        
        if (ImGui::Button("Load Selected ROM", ImVec2(-1, 0))) {
            if (selectedRomIndex >= 0 && selectedRomIndex < availableRoms.size()) {
                selectedRomPath = romsDirectory + "/" + availableRoms[selectedRomIndex];
                romLoadRequested = true;
            }
        }
        
        if (!canLoad) {
            ImGui::EndDisabled();
        }
    }
    
    ImGui::End();
}

void Graphics::RenderKeyboard(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Keyboard", &showKeyboard, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::SeparatorText("Keypad Layout");
    ImGui::TextWrapped("CHIP-8 Keypad layout and PC keyboard mapping:");
    
    ImGui::Spacing();
    ImGui::Text("CHIP-8     PC Keys");
    ImGui::Text("1 2 3 C -> 1 2 3 4");
    ImGui::Text("4 5 6 D -> Q W E R");
    ImGui::Text("7 8 9 E -> A S D F");
    ImGui::Text("A 0 B F -> Z X C V");
    
    ImGui::SeparatorText("Key Status");
    
    // Display keypad state as buttons in proper layout
    const char* keyLabels[16] = {
        "1", "2", "3", "C",
        "4", "5", "6", "D", 
        "7", "8", "9", "E",
        "A", "0", "B", "F"
    };
    
    const int keyMap[16] = {0x1, 0x2, 0x3, 0xC, 0x4, 0x5, 0x6, 0xD, 0x7, 0x8, 0x9, 0xE, 0xA, 0x0, 0xB, 0xF};
    
    // Track which key is currently being pressed by mouse
    static int mouseDownKey = -1;
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int index = row * 4 + col;
            int key = keyMap[index];
            
            if (col > 0) ImGui::SameLine();
            
            // Color the button if key is pressed
            bool pressed = chip8.keypad[key];
            if (pressed) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));
            }
            
            ImGui::Button(keyLabels[index], ImVec2(40, 40));
            
            // Handle mouse press/release for this button
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                // Mouse pressed down on this button
                chip8.keypad[key] = true;
                mouseDownKey = key;
            }
            
            if (pressed) {
                ImGui::PopStyleColor(3);
            }
        }
    }
    
    // Release key when mouse is released anywhere
    if (mouseDownKey >= 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        chip8.keypad[mouseDownKey] = false;
        mouseDownKey = -1;
    }
    
    // Show currently pressed keys
    ImGui::SeparatorText("Active Keys");
    bool anyPressed = false;
    for (int i = 0; i < 16; i++) {
        if (chip8.keypad[i]) {
            if (anyPressed) ImGui::SameLine();
            ImGui::Text("0x%X", i);
            anyPressed = true;
        }
    }
    if (!anyPressed) {
        ImGui::TextDisabled("No keys pressed");
    }
    
    ImGui::End();
}

void Graphics::RenderDisassembly(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - CPU Disassembler", &showDisassembly, ImGuiWindowFlags_NoMove);
    
    static bool followPC = true;
    ImGui::Checkbox("Follow PC", &followPC);
    
    ImGui::SeparatorText("Assembly Code");
    
    ImGui::BeginChild("DisassemblyView", ImVec2(0, -1), true);
    
    // Show more instructions around current PC for context
    int startAddr = followPC ? std::max(0x200, (int)chip8.pc - 40) : 0x200;
    int endAddr = std::min(4095, startAddr + 80); // Show ~40 instructions
    
    // Use monospace font for better alignment
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    
    for (int addr = startAddr; addr < endAddr; addr += 2) {
        if (addr >= 4095) break;
        
        uint16_t instruction = (chip8.memory[addr] << 8) | chip8.memory[addr + 1];
        std::string decoded = DecodeInstruction(instruction);
        
        // Highlight current instruction with arrow and different background
        bool isCurrent = (addr == chip8.pc);
        if (isCurrent) {
            // Highlight the entire line
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::Text("-> 0x%03X | %02X %02X | %-12s  %s", 
                       addr, 
                       chip8.memory[addr], 
                       chip8.memory[addr + 1],
                       (std::to_string((instruction & 0xF000) >> 12) + std::to_string((instruction & 0x0F00) >> 8) + 
                        std::to_string((instruction & 0x00F0) >> 4) + std::to_string(instruction & 0x000F)).c_str(),
                       decoded.c_str());
            ImGui::PopStyleColor();
            
            // Auto-scroll to current instruction when following PC
            if (followPC) {
                ImGui::SetScrollHereY(0.5f);
            }
        } else {
            // Regular instruction display
            ImGui::Text("   0x%03X | %02X %02X | %04X        %s", 
                       addr, 
                       chip8.memory[addr], 
                       chip8.memory[addr + 1],
                       instruction,
                       decoded.c_str());
        }
    }
    
    ImGui::PopFont();
    ImGui::EndChild();
    ImGui::End();
}

void Graphics::RenderDisplay(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Display", &showDisplay, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    // Convert CHIP-8 display to RGBA texture
    uint32_t pixels[64 * 32];
    for (int i = 0; i < 64 * 32; i++) {
        // CHIP-8 uses 1-bit per pixel, convert to ABGR format (0xAABBGGRR)
        pixels[i] = chip8.display[i] ? 0xFFFFFFFF : 0xFF000000; // White (ABGR) or Black (ABGR)
    }
    
    // Update the texture
    SDL_UpdateTexture(displayTexture, nullptr, pixels, 64 * sizeof(uint32_t));
    
    // Set texture filtering to nearest neighbor for pixel-perfect rendering
    SDL_SetTextureScaleMode(displayTexture, SDL_ScaleModeNearest);
    
    // Get texture as ImGui texture ID
    ImTextureID textureID = (ImTextureID)(intptr_t)displayTexture;
    
    // Fixed display size for consistent layout
    ImVec2 displaySize(512, 256); // 8x scale of 64x32
    
    // Center the display
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 centerPos = ImVec2(
        (windowSize.x - displaySize.x) * 0.5f,
        (windowSize.y - displaySize.y) * 0.5f
    );
    
    if (centerPos.x > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + centerPos.x);
    if (centerPos.y > 0) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + centerPos.y);
    
    // Render the display with nearest neighbor filtering for pixel-perfect scaling
    ImGui::Image(textureID, displaySize, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
    
    ImGui::End();
}

std::string Graphics::DecodeInstruction(uint16_t instruction)
{
    Chip8 tempChip8;
    return tempChip8.DecodeAndExecute(instruction);
}

void Graphics::AddToHistory(uint16_t address, uint16_t instruction)
{
    InstructionHistory entry;
    entry.address = address;
    entry.instruction = instruction;
    entry.decoded = DecodeInstruction(instruction);
    
    instructionHistory.push_back(entry);
    
    // Keep only the last MAX_HISTORY entries
    if (instructionHistory.size() > MAX_HISTORY) {
        instructionHistory.erase(instructionHistory.begin());
    }
}

void Graphics::ScanForRoms()
{
    availableRoms.clear();
    
    if (romsDirectory.empty()) {
        return;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(romsDirectory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                // Skip hidden files and system files
                if (filename[0] != '.' && filename != "DS_Store") {
                    availableRoms.push_back(filename);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cout << "Error scanning ROMs directory: " << ex.what() << std::endl;
    }
}

void Graphics::RenderFrame(Chip8& chip8)
{
    // Set background color to black and clear the entire screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Initialize ImGui for a new frame of UI rendering
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Create the top menu bar with Debug and View menus
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("CPU State", NULL, &showCPUState);
            ImGui::MenuItem("Registers", NULL, &showRegisters);
            ImGui::MenuItem("Memory", NULL, &showMemory);
            ImGui::MenuItem("Disassembly", NULL, &showDisassembly);
            ImGui::MenuItem("Display", NULL, &showDisplay);
            ImGui::MenuItem("Controls", NULL, &showControls);
            ImGui::MenuItem("Keyboard", NULL, &showKeyboard);
            ImGui::Separator();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Reset Layout")) {
                SetupWindowLayout();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Render all the CHIP-8 debugger windows in organized layout
    RenderOrganizedLayout(chip8);
    
    // Convert ImGui draw commands to actual graphics and render them
    ImGui::Render(); // Finalize ImGui draw data
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer); // Draw to SDL renderer
    
    // Display the completed frame on screen
    SDL_RenderPresent(renderer);
}

void Graphics::Shutdown()
{
    // Clean up display texture
    if (displayTexture) {
        SDL_DestroyTexture(displayTexture);
        displayTexture = nullptr;
    }
    
    // Cleanup ImGui
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    // Destroy renderer
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    // Destroy window
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    // Quit SDL subsystems
    SDL_Quit();
}
