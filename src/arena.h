#ifndef FIELD_WINDOW_H
#define FIELD_WINDOW_H

#include "std.h"
#include "graphics.h"
#include "shaders.h"

typedef struct sArena {
    Object base;
    float model[16];
    uint32_t vertCount;
    GLuint vao;
    GLuint vbo;
    VertexGlow* vertices;
    
    int wx, wy;
    int width, height;
    char* layout;
} Arena;
static_assert(offsetof(Arena, base) == 0, "Arena struct not aligned for base object");

Arena* createArena(int wx, int wy, int width, int height);
void updateArena(Arena* f);
void drawArena(RenderContext* ctx, Arena* arena);
void destroyArena(Arena* f);

#endif // FIELD_WINDOW_H
