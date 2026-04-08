#ifndef FIELD_H
#define FIELD_H

#include <string>
#include <vector>
#include <SDL3/SDL_render.h>

#define FIELD_WIDTH 12
#define FIELD_HEIGHT 24
#define BLOCK_WIDTH 35
#define BLOCK_HEIGHT 35
#define WALL_COLOR {0.1f, 0.1f, 0.1f, 1.0f}
#define STACK_COLOR {0.5f, 0.5f, 0.5f, 1.0f}
#define START_FIELD_ROW 4

struct sTetrominoe;

typedef struct {
    float x, y;
    std::string fieldLayout;
    std::vector<SDL_Vertex> vertices;
} PlayField;

PlayField createField(float x = 0.0f, float y = 0.0f);
void drawField(SDL_Renderer* renderer, PlayField* f);
void updateField(PlayField* f);
void removeFilledLines(PlayField* f);
int checkForFilledLines(PlayField* f, sTetrominoe* t);

#endif // FIELD_H