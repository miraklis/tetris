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
    int wx, wy;
    float fx, fy;
    int rotationState;
    bool isAlive;
    char shapeLayout[17];
    float centerOffsetX;
    float centerOffsetY;
    Color color;
    TetrominoeType type;
    
    GLuint vao;
    GLuint vbo;
    float proj[16];
    float model[16];
    Vertex vertices[TETROMINOE_VERTICES_COUNT];
} Tetrominoe;

Tetrominoe* createTetrominoe(TetrominoeType type);
void moveTetrominoe(int x, int y, Tetrominoe* t);
void placeTetrominoe(int x, int y, Tetrominoe* t);
void rotateLayout(char* layout);
void rotateTetrominoe(Tetrominoe* t);
void drawTetrominoe(Tetrominoe* t, GameShader* shader);
void destroyTetrominoe(Tetrominoe* t);

#endif // TETROMINOE_H