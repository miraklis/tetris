#include <string>
#include <SDL3/SDL.h>
#include "text.h"

void drawText(SDL_Renderer* renderer, std::string msg, SDL_FColor color, float x, float y, float scale)
{
    if(msg=="")
        return;
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_SetRenderDrawColorFloat(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(renderer, (x /scale), (y / scale), msg.c_str());
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}
