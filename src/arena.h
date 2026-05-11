#ifndef FIELD_WINDOW_H
#define FIELD_WINDOW_H

#include "std.h"
#include "graphics.h"
#include "shaders.h"

typedef struct sArena {
    Object base;
    int wx, wy;
    int width, height;
    char* layout;
    size_t vertCount;

    GLuint vao;
    GLuint vbo;
    float proj[16];
    float model[16];
    VertexGlow* vertices;
} Arena;
static_assert(offsetof(Arena, base) == 0, "Arena struct not aligned for base object");

Arena* createArena(int wx, int wy, int width, int height);
void updateArena(Arena* f);
void drawArena(Arena* f, GlowShader* shader);
void destroyArena(Arena* f);

#endif // FIELD_WINDOW_H
