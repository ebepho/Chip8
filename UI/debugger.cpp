#include "debugger.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iomanip>
#include <sstream>

Debugger::Debugger() : showRegisters(true), showMemory(true), showControls(true), showCPUState(true), showKeyboard(true), showDisassembly(true), renderer(nullptr) {}

bool Debugger::Init(SDL_Window* window, SDL_Renderer* sdlRenderer)
{
    renderer = sdlRenderer; // Store the renderer for later use
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, sdlRenderer);
    ImGui_ImplSDLRenderer2_Init(sdlRenderer);
    
    return true;
}

void Debugger::SetupWindowLayout()
{
    ImGuiIO& io = ImGui::GetIO();
    float windowWidth = io.DisplaySize.x;
    float windowHeight = io.DisplaySize.y;
    float menuBarHeight = ImGui::GetFrameHeight();
    
    // Define layout constants
    const float leftColumnWidth = 300.0f;
    const float rightColumnWidth = 350.0f;
    const float centerColumnWidth = windowWidth - leftColumnWidth - rightColumnWidth - 30.0f; // 30px for padding
    
    // Set window positions and sizes
    // Left column - CPU State and Controls
    ImGui::SetNextWindowPos(ImVec2(10, menuBarHeight + 10));
    ImGui::SetNextWindowSize(ImVec2(leftColumnWidth, (windowHeight - menuBarHeight) * 0.6f - 15));
    
    // Controls below CPU State
    controlsPos = ImVec2(10, menuBarHeight + (windowHeight - menuBarHeight) * 0.6f + 5);
    controlsSize = ImVec2(leftColumnWidth, (windowHeight - menuBarHeight) * 0.4f - 15);
    
    // Center column - Display area (reserved for CHIP-8 screen)
    displayPos = ImVec2(leftColumnWidth + 20, menuBarHeight + 10);
    displaySize = ImVec2(centerColumnWidth, 350);
    
    // Right column - Registers and Keyboard
    registersPos = ImVec2(leftColumnWidth + centerColumnWidth + 30, menuBarHeight + 10);
    registersSize = ImVec2(rightColumnWidth, 200);
    
    keyboardPos = ImVec2(leftColumnWidth + centerColumnWidth + 30, menuBarHeight + 220);
    keyboardSize = ImVec2(rightColumnWidth, 200);
    
    // Memory window - bottom spanning most of the width
    memoryPos = ImVec2(10, windowHeight - 300);
    memorySize = ImVec2(windowWidth - 20, 280);
}

void Debugger::RenderOrganizedLayout(Chip8& chip8)
{
    // CPU State Window (top-left)
    if (showCPUState) {
        ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetFrameHeight() + 10), ImGuiCond_FirstUseEver);
        RenderCPUState(chip8);
    }
    
    // Controls Window (bottom-left)
    if (showControls) {
        ImGui::SetNextWindowPos(controlsPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(controlsSize, ImGuiCond_FirstUseEver);
        RenderControls(chip8);
    }
    
    // Registers Window (top-right)
    if (showRegisters) {
        ImGui::SetNextWindowPos(registersPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(registersSize, ImGuiCond_FirstUseEver);
        RenderRegisters(chip8);
    }
    
    // Keyboard Window (middle-right)
    if (showKeyboard) {
        ImGui::SetNextWindowPos(keyboardPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(keyboardSize, ImGuiCond_FirstUseEver);
        RenderKeyboard(chip8);
    }
    
    // Memory Window (bottom spanning)
    if (showMemory) {
        ImGui::SetNextWindowPos(memoryPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(memorySize, ImGuiCond_FirstUseEver);
        RenderMemory(chip8);
    }
    
    // Disassembly Window (center-right)
    if (showDisassembly) {
        ImGui::SetNextWindowPos(ImVec2(displayPos.x + displaySize.x + 10, displayPos.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_FirstUseEver);
        RenderDisassembly(chip8);
    }
}

void Debugger::ProcessEvent(SDL_Event* event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void Debugger::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Debugger::Render(Chip8& chip8)
{
    static bool show_demo_window = false;
    static bool firstTime = true;
    
    // Set up initial window positions and sizes on first run
    if (firstTime) {
        SetupWindowLayout();
        firstTime = false;
    }
    
    // Show the main menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("CPU State", NULL, &showCPUState);
            ImGui::MenuItem("Registers", NULL, &showRegisters);
            ImGui::MenuItem("Memory", NULL, &showMemory);
            ImGui::MenuItem("Disassembly", NULL, &showDisassembly);
            ImGui::MenuItem("Controls", NULL, &showControls);
            ImGui::MenuItem("Keyboard", NULL, &showKeyboard);
            ImGui::Separator();
            ImGui::MenuItem("Demo Window", NULL, &show_demo_window);
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
    
    // Demo window for reference
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    
    // Render all debug windows with organized layout
    RenderOrganizedLayout(chip8);
}

void Debugger::RenderCPUState(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - CPU State", &showCPUState, ImGuiWindowFlags_NoResize);
    
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
    if (chip8.sp > 0) {
        ImGui::Text("Stack Top:");
        for (int i = chip8.sp - 1; i >= 0 && i >= chip8.sp - 4; i--) {
            ImGui::Text("  [%d]: 0x%04X", i, chip8.stack[i]);
        }
        if (chip8.sp > 4) {
            ImGui::Text("  ... (%d more)", chip8.sp - 4);
        }
    } else {
        ImGui::TextDisabled("Stack empty");
    }
    
    ImGui::End();
}

void Debugger::RenderRegisters(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Registers", &showRegisters, ImGuiWindowFlags_NoResize);
    
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

void Debugger::RenderMemory(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Memory", &showMemory);
    
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

void Debugger::RenderControls(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Controls", &showControls, ImGuiWindowFlags_NoResize);
    
    static bool isPaused = false;
    static bool stepMode = false;
    
    ImGui::SeparatorText("Execution Control");
    
    // Main control buttons
    if (ImGui::Button("Reset", ImVec2(80, 30))) {
        // Reset the chip8 system
        chip8 = Chip8(); // This will call the constructor to reset
        // TODO: Reload the current ROM
    }
    
    ImGui::SameLine();
    if (ImGui::Button(isPaused ? "Resume" : "Pause", ImVec2(80, 30))) {
        isPaused = !isPaused;
        // TODO: Implement pause functionality in main loop
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Step", ImVec2(80, 30))) {
        // TODO: Execute one instruction
        stepMode = true;
    }
    
    ImGui::SeparatorText("Emulation Settings");
    
    ImGui::Text("Speed:");
    static float emulationSpeed = 1.0f;
    ImGui::SliderFloat("##Speed", &emulationSpeed, 0.1f, 10.0f, "%.1fx");
    
    ImGui::Text("Display Scale:");
    static int displayScale = 10;
    ImGui::SliderInt("##Scale", &displayScale, 1, 20);
    
    ImGui::SeparatorText("ROM Info");
    static char romPath[256] = "No ROM loaded";
    ImGui::TextWrapped("Current ROM:");
    ImGui::TextWrapped("%s", romPath);
    
    if (ImGui::Button("Load ROM", ImVec2(-1, 0))) {
        // TODO: Implement ROM file browser
    }
    
    ImGui::End();
}

void Debugger::RenderKeyboard(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Keyboard", &showKeyboard, ImGuiWindowFlags_NoResize);
    
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
            
            if (ImGui::Button(keyLabels[index], ImVec2(40, 40))) {
                // Toggle key state (for testing)
                chip8.keypad[key] = !chip8.keypad[key];
            }
            
            if (pressed) {
                ImGui::PopStyleColor(3);
            }
        }
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

void Debugger::RenderDisassembly(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Disassembly", &showDisassembly);
    
    ImGui::SeparatorText("Code Disassembly");
    
    // Show instructions around current PC
    int startAddr = std::max(0x200, (int)chip8.pc - 20);
    int endAddr = std::min(4096, (int)chip8.pc + 20);
    
    ImGui::BeginChild("DisassemblyView", ImVec2(0, -1), true);
    
    for (int addr = startAddr; addr < endAddr; addr += 2) {
        if (addr >= 4096 - 1) break;
        
        uint16_t instruction = (chip8.memory[addr] << 8) | chip8.memory[addr + 1];
        std::string decoded = DecodeInstruction(instruction);
        
        // Highlight current instruction
        bool isCurrent = (addr == chip8.pc);
        if (isCurrent) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::Text("=> 0x%04X: %04X  %s", addr, instruction, decoded.c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::Text("   0x%04X: %04X  %s", addr, instruction, decoded.c_str());
        }
    }
    
    ImGui::EndChild();
    ImGui::End();
}

std::string Debugger::DecodeInstruction(uint16_t instruction)
{
    uint8_t firstNibble = (instruction & 0xF000) >> 12;
    uint8_t x = (instruction & 0x0F00) >> 8;
    uint8_t y = (instruction & 0x00F0) >> 4;
    uint8_t n = instruction & 0x000F;
    uint8_t nn = instruction & 0x00FF;
    uint16_t nnn = instruction & 0x0FFF;
    
    switch (firstNibble) {
        case 0x0:
            if (instruction == 0x00E0) return "CLS";
            if (instruction == 0x00EE) return "RET";
            return "SYS " + std::to_string(nnn);
            
        case 0x1:
            return "JP " + std::to_string(nnn);
            
        case 0x2:
            return "CALL " + std::to_string(nnn);
            
        case 0x3:
            return "SE V" + std::to_string(x) + ", " + std::to_string(nn);
            
        case 0x4:
            return "SNE V" + std::to_string(x) + ", " + std::to_string(nn);
            
        case 0x5:
            return "SE V" + std::to_string(x) + ", V" + std::to_string(y);
            
        case 0x6:
            return "LD V" + std::to_string(x) + ", " + std::to_string(nn);
            
        case 0x7:
            return "ADD V" + std::to_string(x) + ", " + std::to_string(nn);
            
        case 0xA:
            return "LD I, " + std::to_string(nnn);
            
        case 0xD:
            return "DRW V" + std::to_string(x) + ", V" + std::to_string(y) + ", " + std::to_string(n);
            
        default:
            return "UNK";
    }
}

void Debugger::EndFrame()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void Debugger::Shutdown()
{
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
