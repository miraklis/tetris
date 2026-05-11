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
    VertexSimple backgroundVertices[6];
    VertexSimple barVertices[6];
    Text** items;

    GLuint backVao;
    GLuint backVbo;
    GLuint barVao;
    GLuint barVbo;
    float proj[16];
    float menuModel[16];
    float barModel[16];
} Menu;

Menu* createMenu(char* items, float x, float y, float fontSize);
void handleMenuInput(const bool* currentKeyStates, Menu* menu);
void drawMenu(Menu* menu, SimpleShader* gameShader, ColoredTextureShader* uiShader);
void destroyMenu(Menu* menu);

#endif