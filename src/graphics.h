#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLES3/gl3.h>
#include <SDL3/SDL.h>

#define QUAD_VERTICES 6

#define FIELD_WIDTH 12
#define FIELD_HEIGHT 22

#define FONT "assets/PressStart2P-Regular.ttf"

typedef enum {
    VAO_LAYOUT_SIMPLE,
    VAO_LAYOUT_GLOW,
    VAO_LAYOUT_GLYPH,
    VAO_LAYOUT_COUNT
} VertexLayout;

typedef struct sVertexSimple {
    float x, y;
    float r, g, b, a;
} VertexSimple;

typedef struct sVertexGlow {
    float x, y;
    float r, g, b, a;
    float glow;
} VertexGlow;

typedef struct sVertexGlyph {
    float x,y;
    float u,v;
} VertexGlyph;

typedef struct sColor {
    float r, g, b, a;
} Color;

typedef struct sPalette {
    Color colorBlack;
    Color colorGray10;
    Color colorGray20;
    Color colorGray60;
    Color colorRed;
    Color colorBlue;
    Color colorGreen;
    Color colorYellow;
    Color colorPink;
    Color colorCyan;
    Color colorPurple;
    Color colorWhite;
    Color transparentBlack80;
} ColorsPalette;

typedef enum eObjectTypes {
    OBJ_TYPE_Window = 0,
    OBJ_TYPE_Tetrominoe,
    OBJ_TYPE_Text,
    OBJ_TYPE_Arena,
    OBJ_TYPE_Count
} ObjectTypes;

typedef struct sObject {
    float x, y;
    float width, height;
    ObjectTypes id;
} Object;
extern ColorsPalette palette;

typedef struct sGraphics {
    const SDL_DisplayMode* dm;
    SDL_DisplayID* displays;
    SDL_Window* window;
    SDL_GLContext context;
    float screenWidth;
    float screenHeight;
    uint32_t blockWidth;
    uint32_t blockHeight;
} Graphics;
extern Graphics graphics;

void initializeGraphics(void);
void destroyGraphics(void);

void setupVertexLayout(GLuint vao, GLuint vbo, VertexLayout layout);


void orthoMatrix(float left, float right, float bottom, float top, float near, float far, float* m);
void translateMatrix(float x, float y, float* m);
void rotateMatrix(float angle, float* m);
void multiplyMatrix4x4(const float* a, const float* b, float* out);

void updateVerticesSimple(VertexSimple* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor);
void updateVerticesGlow(VertexGlow* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor, float glow);

#endif