#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Graphics
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

public:
    Graphics();
    ~Graphics();

    bool Init(const char* title, int width, int height, int textureWidth, int textureHeight);
    void Update(void const* buffer, int width, int height, int pitch);
};