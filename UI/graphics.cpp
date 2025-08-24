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

    // Clear the renderer and copy the texture to it, then present
    SDL_RenderClear(renderer);

    // Copy the entire texture to the entire window
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Present the updated renderer
    SDL_RenderPresent(renderer);
}


