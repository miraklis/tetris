#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLES3/gl3.h>
#include <SDL3/SDL.h>

#define QUAD_VERTICES 6

#define BLOCK_WIDTH 34
#define BLOCK_HEIGHT 34
#define FIELD_WIDTH 12
#define FIELD_HEIGHT 25
#define FIELD_START_ROW 4

#define FONT "assets/PressStart2P-Regular.ttf"

typedef struct sVertex {
    float x, y;
    float r, g, b, a;
} Vertex;

typedef struct sColor {
    float r, g, b, a;
} Color;

extern const SDL_DisplayMode* dm;

void orthoMatrix(float left, float right, float bottom, float top, float near, float far, float* m);
void translateMatrix(float x, float y, float* m);
void rotateMatrix(float angle, float* m);

void multiplyMatrix4x4(const float* a, const float* b, float* out);

#endif