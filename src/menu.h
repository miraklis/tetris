#ifndef MENU_H
#define MENU_H

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

typedef struct {
    float x;
    float y;
    float w;
    float h;
    float scale;
    SDL_Vertex vertices[12];
    std::vector<std::string> items;
    int selectedIndex;
    int* actions;
    void(*callback)(int, int*);
} Menu;

Menu createMenu(std::vector<std::string>& items, int x, int y, float scale, int* actions, void(*callback)(int, int*));
void handleMenuInput(SDL_Event& ev, Menu& menu);
void drawMenu(SDL_Renderer* renderer, Menu* menu);

#endif