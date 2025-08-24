#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <vector>
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
    std::string DecodeInstruction(uint16_t instruction);
    void AddToHistory(uint16_t address, uint16_t instruction);
    
public:
    // Control interface for main loop
    bool IsPaused() const { return isPaused; }
    bool IsStepMode() const { return stepMode; }
    bool ShouldReset() const { return shouldReset; }
    void ResetHandled() { shouldReset = false; }
    void StepHandled() { stepMode = false; }
    void SetRomPath(const std::string& path) { currentRomPath = path; }
};