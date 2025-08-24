#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "chip8.h"

class Debugger 
{
private:
    bool showRegisters;
    bool showMemory;
    bool showControls;
    bool showCPUState;
    bool showKeyboard;
    SDL_Renderer* renderer;
    
public:
    Debugger();
    
    bool Init(SDL_Window* window, SDL_Renderer* renderer);
    void ProcessEvent(SDL_Event* event);
    void NewFrame();
    void Render(Chip8& chip8);
    void EndFrame();
    void Shutdown();
    
private:
    void RenderCPUState(Chip8& chip8);
    void RenderRegisters(Chip8& chip8);
    void RenderMemory(Chip8& chip8);
    void RenderControls(Chip8& chip8);
    void RenderKeyboard(Chip8& chip8);
    std::string DecodeInstruction(uint16_t instruction);
};