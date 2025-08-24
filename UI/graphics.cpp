#include "graphics.h"
#include <iostream>

Graphics::Graphics() {
}

Graphics::~Graphics() {
    // Destroy window
    if (window) {
        SDL_DestroyWindow(window);
    }
    window = NULL;
    
    // Destroy renderer
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    renderer = NULL;

    // Destroy texture
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    texture = NULL;
    // Quit SDL subsystems
    SDL_Quit();
}

bool Graphics::Init(const char* title, int width, int height, int textureWidth, int textureHeight) {

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

    // Create texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
    if (!texture) {
        std::cout << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

// Update the graphics -> https://austinmorlan.com/posts/chip8_emulator/
void Graphics::Update(void const* buffer, int width, int height, int pitch)
{
    // Update the texture with the new pixel data
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);

    // Copy the entire texture to a portion of the window (leave space for ImGui)
    SDL_Rect displayRect = { 10, 10, 640, 320 }; // Fixed size display area
    SDL_RenderCopy(renderer, texture, nullptr, &displayRect);
}

void Graphics::Clear()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Graphics::Present()
{
    SDL_RenderPresent(renderer);
}


