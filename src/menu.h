#ifndef MENU_H
#define MENU_H

#include "graphics.h"
#include "shaders.h"
#include "text.h"

//#define MENU_VERTICES_COUNT 12

typedef struct {
    float x, y;
    float w, h;
    float fontSize;
    int selectedIndex;
    int action;
    bool visible;
    size_t itemsCnt;
    Vertex backgroundVertices[6];
    Vertex barVertices[6];
    Text** items;

    GLuint backVao;
    GLuint barVao;
    float proj[16];
    float menuModel[16];
    float barModel[16];
} Menu;

Menu* createMenu(char* items, float x, float y, float fontSize);
void handleMenuInput(const bool* currentKeyStates, Menu* menu);
void drawMenu(Menu* menu, GameShader* gameShader, ColoredTextureShader* uiShader);
void destroyMenu(Menu* menu);

#endif