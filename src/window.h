#ifndef WINDOW_H
#define WINDOW_H

#include "std.h"
#include "graphics.h"
#include "shaders.h"

typedef struct sWindow {
    Object base;
    float proj[16];
    float model[16];
    int32_t wx, wy;
    uint32_t width, height;
    uint32_t vertCount;
    GLuint vao;
    GLuint vbo;
    VertexSimple* vertices;
} Window;
static_assert(offsetof(Window, base) == 0, "Window struct not aligned for base object");

Window* createWindow(int wx, int wy, int width, int height, Color* backgroundColor, bool hasBorder);
void drawWindow(Window* f, SimpleShader* shader);
void addObjectToWindow(Window* f, Object* o);
void destroyWindow(Window* f);

#endif // WINDOW_H
