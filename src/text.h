#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <SDL3/SDL.h>

void drawText(SDL_Renderer* renderer, std::string msg, SDL_FColor color, float x, float y, float scale = 1.0f);

#endif // TEXT_H