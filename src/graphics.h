#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLES3/gl3.h>
#include <SDL3/SDL.h>

#define QUAD_VERTICES 6

#define BLOCK_WIDTH 34
#define BLOCK_HEIGHT 34
#define FIELD_WIDTH 12
#define FIELD_HEIGHT 22

#define FONT "assets/PressStart2P-Regular.ttf"

typedef struct sVertex {
    float x, y;
    float r, g, b, a;
    float glow;
} Vertex;

typedef struct sColor {
    float r, g, b, a;
} Color;

extern const SDL_DisplayMode* dm;

extern Color colorBlack;
extern Color colorGray10;
extern Color colorGray20;
extern Color colorGray60;
extern Color colorRed;
extern Color colorBlue;
extern Color colorGreen;
extern Color colorYellow;
extern Color colorPink;
extern Color colorCyan;
extern Color colorPurple;
extern Color colorWhite;
extern Color transparentBlack80;

void orthoMatrix(float left, float right, float bottom, float top, float near, float far, float* m);
void translateMatrix(float x, float y, float* m);
void rotateMatrix(float angle, float* m);
void multiplyMatrix4x4(const float* a, const float* b, float* out);

void updateBlockVertices(Vertex* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor, float glow);

#endif