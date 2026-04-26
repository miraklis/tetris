#ifndef MENU_H
#define MENU_H

#include <GLES3/gl3.h>
#include <SDL3/SDL_events.h>
#include "graphics.h"
#include "shaders.h"
#include "text.h"

#define MENU_VERTICES_COUNT 12

typedef struct {
    float x, y;
    float w, h;
    float fontSize;
    int selectedIndex;
    int action;
    bool visible;
    size_t itemsCnt;
    Vertex vertices[12];
    Text* items;

    GLuint vao;
    GLuint vbo;
    float proj[16];
    float model[16];
} Menu;

Menu* createMenu(char* items, float x, float y, float fontSize);
void handleMenuInput(const bool* currentKeyStates, Menu* menu);
void drawMenu(Menu* menu, GameShader* gameShader, ColoredTextureShader* uiShader);//GLuint program);

#endif