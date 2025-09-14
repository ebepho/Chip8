#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <filesystem>
#include "chip8.h"
#include "imgui.h"

class Graphics 
{
    // SDL objects
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* displayTexture;

    // Menu toggles for various windows
    bool showCPUState;
    bool showRegisters;
    bool showMemory;
    bool showDisassembly;
    bool showDisplay;
    bool showControls;
    bool showKeyboard;

    // Control state
    bool isReset;
    bool isPaused;
    bool isStep;
    std::string currentRomPath;
    
    // ROM selection
    bool romLoadRequested;
    std::string selectedRomPath;
    std::vector<std::string> availableRoms;
    std::string romsDirectory;
    int selectedRomIndex;
    void ScanForRoms(); 
    
    // Instruction history for debugging
    struct InstructionHistory {
        uint16_t address;
        uint16_t instruction;
        std::string decoded;
    };
    std::vector<InstructionHistory> instructionHistory; // Make this double linked list later
    static const size_t MAX_HISTORY = 100;
    std::string DecodeInstruction(uint16_t instruction);
    void AddToHistory(uint16_t address, uint16_t instruction);

    // Layout positioning
    ImVec2 cpuStatePos, cpuStateSize;
    ImVec2 controlsPos, controlsSize;
    ImVec2 displayPos, displaySize;
    ImVec2 registersPos, registersSize;
    ImVec2 keyboardPos, keyboardSize;
    ImVec2 memoryPos, memorySize;
    ImVec2 disassemblyPos, disassemblySize;

    // Private helper methods for rendering and layout
    void SetupWindowLayout();
    void RenderOrganizedLayout(Chip8& chip8);
    void RenderCPUState(Chip8& chip8);
    void RenderRegisters(Chip8& chip8);
    void RenderMemory(Chip8& chip8);
    void RenderControls(Chip8& chip8);
    void RenderKeyboard(Chip8& chip8);
    void RenderDisassembly(Chip8& chip8);
    void RenderDisplay(Chip8& chip8);  
      
public:
    Graphics();
    
    // Initialize and manage the debugger UI
    bool Init(int width, int height);

    // Event processing
    void ProcessEvent(SDL_Event* event);
    bool HandleInput(SDL_Event* event, Chip8& chip8);

    // Frame management
    void RenderFrame(Chip8& chip8); // Complete frame rendering - replaces NewFrame, Render, EndFrame, Clear, Present
    void Shutdown();
    
    // Getters for SDL objects (if needed by main loop)
    SDL_Window* GetWindow() const { return window; }
    SDL_Renderer* GetRenderer() const { return renderer; }

    // Getters for control state in main loop
    bool IsPaused() const { return isPaused; }
    bool IsStepMode() const { return isStep; }
    bool ShouldReset() const { return isReset; }
    std::string GetSelectedRomPath() const { return selectedRomPath; }
    bool IsRomLoadRequested() const { return romLoadRequested; }

    // Setters for control state in main loop
    void ResetHandled() { isReset = false; }
    void StepHandled() { isStep = false; }
    void RomLoadHandled() { romLoadRequested = false; }
    void SetRomPath(const std::string& path) { currentRomPath = path; }
    void SetRomsDirectory(const std::string& dir) { romsDirectory = dir; ScanForRoms(); } 
};