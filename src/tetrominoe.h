#ifndef TETROMINOE_H
#define TETROMINOE_H

#include "field.h"
#include <string>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

#if DRAWFULL
    #define VERTICES_COUNT 480
#else
    #define VERTICES_COUNT 120
#endif

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
    float x,y;
    bool isAlive;
    SDL_FColor color;
    SDL_Vertex vertices[VERTICES_COUNT];
    std::string shapeLayout;
    TetrominoeType type;
} Tetrominoe;

Tetrominoe createTetrominoe(TetrominoeType type);
void attachTetrominoeToField(Tetrominoe* t, PlayField* f, int x = 0, int y = 0);
void moveTetrominoe(int x, int y, Tetrominoe* t);
void placeTetrominoe(Tetrominoe* t, float x, float y);
void rotateTetrominoe(Tetrominoe* t);
void drawTetrominoe(SDL_Renderer* renderer, Tetrominoe* t);
void rotateLayout(std::string* layout);
bool checkCollissions(Tetrominoe* t, PlayField* f, int offsetX, int offsetY, bool rotated);

#endif // TETROMINOE_H