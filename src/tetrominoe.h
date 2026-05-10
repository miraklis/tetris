#ifndef TETROMINOE_H
#define TETROMINOE_H

#include "std.h"
#include "graphics.h"
#include "shaders.h"

#define TETROMINOE_VERTICES_COUNT 48

typedef enum {
    TT_Line = 0,
    TT_Square,
    TT_T,
    TT_L1,
    TT_L2,
    TT_h1,
    TT_h2,
    TT_Count
} TetrominoeType;

typedef struct sTetrominoe {
    Object base;
    Vertex vertices[TETROMINOE_VERTICES_COUNT];
    float proj[16];
    float model[16];
    char shapeLayout[17];
    Color color;
    float centerOffsetX;
    float centerOffsetY;
    int wx, wy;
    GLuint vao;
    GLuint vbo;
    int rotationState;
    TetrominoeType type;
    bool isAlive;
} Tetrominoe;
static_assert(offsetof(Tetrominoe, base)==0, "Tetrominoe struct not aligned for base object");

Tetrominoe* createTetrominoe(TetrominoeType type);
void moveTetrominoe(int x, int y, Tetrominoe* t);
void placeTetrominoe(int x, int y, Tetrominoe* t);
void rotateLayout(char* layout);
void rotateTetrominoe(Tetrominoe* t);
void drawTetrominoe(Tetrominoe* t, GameShader* shader);
void destroyTetrominoe(Tetrominoe* t);

#endif // TETROMINOE_H