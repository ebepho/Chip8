#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <filesystem>
#include "chip8.h"
#include "imgui.h"

class Debugger 
{
private:
    bool showRegisters;
    bool showMemory;
    bool showControls;
    bool showCPUState;
    bool showKeyboard;
    bool showDisassembly;
    bool showDisplay;
    SDL_Renderer* renderer;
    SDL_Texture* displayTexture;
    
    // Instruction history for debugging
    struct InstructionHistory {
        uint16_t address;
        uint16_t instruction;
        std::string decoded;
    };
    std::vector<InstructionHistory> instructionHistory;
    static const size_t MAX_HISTORY = 100;
    
    // Control state
    bool isPaused;
    bool stepMode;
    bool shouldReset;
    std::string currentRomPath;
    
    // ROM selection
    bool romLoadRequested;
    std::string selectedRomPath;
    std::vector<std::string> availableRoms;
    std::string romsDirectory;
    int selectedRomIndex;
    
    // Layout positioning
    ImVec2 cpuStatePos, cpuStateSize;
    ImVec2 controlsPos, controlsSize;
    ImVec2 displayPos, displaySize;
    ImVec2 registersPos, registersSize;
    ImVec2 keyboardPos, keyboardSize;
    ImVec2 memoryPos, memorySize;
    ImVec2 disassemblyPos, disassemblySize;
    
public:
    Debugger();
    
    bool Init(SDL_Window* window, SDL_Renderer* renderer);
    void ProcessEvent(SDL_Event* event);
    void NewFrame();
    void Render(Chip8& chip8);
    void EndFrame();
    void Shutdown();
    
private:
    void SetupWindowLayout();
    void RenderOrganizedLayout(Chip8& chip8);
    void RenderCPUState(Chip8& chip8);
    void RenderRegisters(Chip8& chip8);
    void RenderMemory(Chip8& chip8);
    void RenderControls(Chip8& chip8);
    void RenderKeyboard(Chip8& chip8);
    void RenderDisassembly(Chip8& chip8);
    void RenderDisplay(Chip8& chip8);
    void ScanForRoms();
    std::string DecodeInstruction(uint16_t instruction);
    void AddToHistory(uint16_t address, uint16_t instruction);
    
public:
    // Control interface for main loop
    bool IsPaused() const { return isPaused; }
    bool IsStepMode() const { return stepMode; }
    bool ShouldReset() const { return shouldReset; }
    bool IsRomLoadRequested() const { return romLoadRequested; }
    void ResetHandled() { shouldReset = false; }
    void StepHandled() { stepMode = false; }
    void RomLoadHandled() { romLoadRequested = false; }
    void SetRomPath(const std::string& path) { currentRomPath = path; }
    void SetRomsDirectory(const std::string& dir) { romsDirectory = dir; ScanForRoms(); }
    std::string GetSelectedRomPath() const { return selectedRomPath; }
};