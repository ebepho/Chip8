#include "debugger.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iomanip>
#include <sstream>

Debugger::Debugger() : showRegisters(true), showMemory(true), showControls(true), showCPUState(true), showKeyboard(true), renderer(nullptr) {}

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
    
    // Show the main menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("CPU State", NULL, &showCPUState);
            ImGui::MenuItem("Registers", NULL, &showRegisters);
            ImGui::MenuItem("Memory", NULL, &showMemory);
            ImGui::MenuItem("Controls", NULL, &showControls);
            ImGui::MenuItem("Keyboard", NULL, &showKeyboard);
            ImGui::Separator();
            ImGui::MenuItem("Demo Window", NULL, &show_demo_window);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    // Demo window for reference
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    
    // CPU State Window
    if (showCPUState) {
        RenderCPUState(chip8);
    }
    
    // Registers Window
    if (showRegisters) {
        RenderRegisters(chip8);
    }
    
    // Memory Window
    if (showMemory) {
        RenderMemory(chip8);
    }
    
    // Controls Window
    if (showControls) {
        RenderControls(chip8);
    }
    
    // Keyboard Window
    if (showKeyboard) {
        RenderKeyboard(chip8);
    }
}

void Debugger::RenderCPUState(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - CPU State", &showCPUState);
    
    // Get current instruction
    uint16_t instruction = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
    
    ImGui::Text("PC: 0x%04X", chip8.pc);
    ImGui::Text("SP: %d", chip8.sp);
    ImGui::Text("I: 0x%04X", chip8.index);
    ImGui::Separator();
    
    ImGui::Text("Current Instruction: 0x%04X", instruction);
    
    // Decode instruction for display
    std::string decoded = DecodeInstruction(instruction);
    ImGui::Text("Decoded: %s", decoded.c_str());
    
    ImGui::Separator();
    ImGui::Text("Delay Timer: %d", chip8.delayTimer);
    ImGui::Text("Sound Timer: %d", chip8.soundTimer);
    
    ImGui::End();
}

void Debugger::RenderRegisters(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Registers", &showRegisters);
    
    // Display registers in a 4x4 grid
    ImGui::Columns(4, "RegisterColumns");
    
    for (int i = 0; i < 16; i++) {
        ImGui::Text("V%X", i);
        ImGui::SameLine();
        ImGui::Text("0x%02X", chip8.registers[i]);
        ImGui::NextColumn();
        
        if ((i + 1) % 4 == 0) {
            ImGui::Separator();
        }
    }
    
    ImGui::Columns(1);
    ImGui::End();
}

void Debugger::RenderMemory(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Memory", &showMemory);
    
    static int memoryStart = 0x200; // Start at ROM area
    ImGui::SliderInt("Start Address", &memoryStart, 0, 4096-16, "0x%04X");
    
    ImGui::Separator();
    
    // Display memory in hex editor style
    ImGui::BeginChild("MemoryView");
    
    for (int i = 0; i < 16; i++) {
        int addr = memoryStart + i * 16;
        if (addr >= 4096) break;
        
        ImGui::Text("0x%04X:", addr);
        ImGui::SameLine();
        
        // Hex values
        for (int j = 0; j < 16 && (addr + j) < 4096; j++) {
            ImGui::SameLine();
            uint8_t byte = chip8.memory[addr + j];
            
            // Highlight PC location
            if (addr + j == chip8.pc) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("%02X", byte);
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%02X", byte);
            }
        }
        
        ImGui::SameLine();
        ImGui::Text("|");
        ImGui::SameLine();
        
        // ASCII representation
        for (int j = 0; j < 16 && (addr + j) < 4096; j++) {
            ImGui::SameLine();
            uint8_t byte = chip8.memory[addr + j];
            char c = (byte >= 32 && byte < 127) ? byte : '.';
            ImGui::Text("%c", c);
        }
    }
    
    ImGui::EndChild();
    ImGui::End();
}

void Debugger::RenderControls(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Controls", &showControls);
    
    static bool isPaused = false;
    static bool stepMode = false;
    
    if (ImGui::Button("Reset")) {
        // Reset the chip8 system
        chip8 = Chip8(); // This will call the constructor to reset
        // TODO: Reload the current ROM
    }
    
    ImGui::SameLine();
    if (ImGui::Button(isPaused ? "Resume" : "Pause")) {
        isPaused = !isPaused;
        // TODO: Implement pause functionality in main loop
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Step")) {
        // TODO: Execute one instruction
        stepMode = true;
    }
    
    ImGui::Separator();
    
    ImGui::Text("Emulation Speed:");
    static float emulationSpeed = 1.0f;
    ImGui::SliderFloat("Speed", &emulationSpeed, 0.1f, 10.0f, "%.1fx");
    
    ImGui::Separator();
    
    ImGui::Text("Display Size:");
    static int displaySize = 10;
    ImGui::SliderInt("Scale", &displaySize, 1, 20);
    
    ImGui::End();
}

void Debugger::RenderKeyboard(Chip8& chip8)
{
    ImGui::Begin("CHIP-8 - Keyboard", &showKeyboard);
    
    ImGui::Text("CHIP-8 Keypad Layout:");
    ImGui::Text("1 2 3 C    Maps to:    1 2 3 4");
    ImGui::Text("4 5 6 D                Q W E R");
    ImGui::Text("7 8 9 E                A S D F");
    ImGui::Text("A 0 B F                Z X C V");
    
    ImGui::Separator();
    
    // Display keypad state as buttons
    const char* keyLabels[16] = {
        "1", "2", "3", "C",
        "4", "5", "6", "D", 
        "7", "8", "9", "E",
        "A", "0", "B", "F"
    };
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int key = row * 4 + col;
            
            if (col > 0) ImGui::SameLine();
            
            // Color the button if key is pressed
            if (chip8.keypad[key]) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
            }
            
            if (ImGui::Button(keyLabels[key], ImVec2(30, 30))) {
                // Toggle key state (for testing)
                chip8.keypad[key] = !chip8.keypad[key];
            }
            
            if (chip8.keypad[key]) {
                ImGui::PopStyleColor();
            }
        }
    }
    
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
