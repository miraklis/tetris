#ifndef FIELD_WINDOW_H
#define FIELD_WINDOW_H

#include "graphics.h"
#include "shaders.h"

typedef struct sFieldWindow {
    int wx, wy;
    int width, height;
    float rx, ry;
    char* layout;
    size_t vertCount;

    GLuint vao;
    GLuint vbo;
    float proj[16];
    float model[16];
    Vertex* vertices;
} FieldWindow;

FieldWindow* createFieldWindow(int wx, int wy, int width, int height, bool filledBackground, Color* backgroundColor);
void updateFieldWindow(FieldWindow* f);
void drawFieldWindow(FieldWindow* f, GameShader* shader);

#endif // FIELD_WINDOW_H
