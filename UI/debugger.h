#pragma once
#include <SDL2/SDL.h>
#include <string>
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
    SDL_Renderer* renderer;
    
    // Layout positioning
    ImVec2 controlsPos, controlsSize;
    ImVec2 displayPos, displaySize;
    ImVec2 registersPos, registersSize;
    ImVec2 keyboardPos, keyboardSize;
    ImVec2 memoryPos, memorySize;
    
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
    std::string DecodeInstruction(uint16_t instruction);
};