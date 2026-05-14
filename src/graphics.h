#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <GLES3/gl3.h>
#include <SDL3/SDL.h>
#include "shaders.h"

#define QUAD_VERTICES 6

#define FIELD_WIDTH 12
#define FIELD_HEIGHT 22

#define FONT "assets/PressStart2P-Regular.ttf"

typedef enum eVertexLayout {
    VAO_LAYOUT_SIMPLE,
    VAO_LAYOUT_GLOW,
    VAO_LAYOUT_GLYPH,
    VAO_LAYOUT_COUNT
} VertexLayout;

typedef enum eRenderableType {
    RENDERABLE_IMAGE,
    RENDERABLE_WINDOW,
    RENDERABLE_ARENA,
    RENDERABLE_TETROMINOE,
    RENDERABLE_TEXT,
    RENDERABLE_MENU,
    RENDERABLE_COUNT
} RenderableType;

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
extern ColorsPalette palette;

typedef enum eObjectType {
    OBJ_TYPE_Window = 0,
    OBJ_TYPE_Tetrominoe,
    OBJ_TYPE_Text,
    OBJ_TYPE_Arena,
    OBJ_TYPE_Count
} ObjectType;

typedef struct sObject {
    float x, y;
    float width, height;
    ObjectType id;
} Object;

typedef struct sRenderable {
    void* data;
    RenderableType type;
} Renderable;

typedef struct sRenderContext {
    GLuint lastProgram;

    SimpleShader* simpleShader;
    GlowShader* glowShader;
    TextureShader* textureShader;
    ColoredTextureShader* coloredTextureShader;

    float projection[16];

    Renderable* queue;
    uint32_t queueCount;
    uint32_t queueCapacity;

} RenderContext;

typedef struct sGraphics {
    const SDL_DisplayMode* dm;
    SDL_DisplayID* displays;
    SDL_Window* window;
    SDL_GLContext context;
    RenderContext* renderContext;
    float screenWidth;
    float screenHeight;
    uint32_t blockWidth;
    uint32_t blockHeight;
} Graphics;
extern Graphics graphics;

void initializeGraphics(void);
void destroyGraphics(void);

RenderContext* createRenderContex(void);
void destroyRenderContext(RenderContext* ctx);
void renderContextBeginFrame(RenderContext* ctx);
void renderContextQueueOject(RenderContext* ctx, RenderableType type, void* data);
void renderContextFlush(RenderContext* ctx);

// Helper Functions
void orthoMatrix(float left, float right, float bottom, float top, float near, float far, float* m);
void translateMatrix(float x, float y, float* m);
void rotateMatrix(float angle, float* m);
void multiplyMatrix4x4(const float* a, const float* b, float* out);
void setupVertexLayout(GLuint vao, GLuint vbo, VertexLayout layout);
void updateVerticesSimple(VertexSimple* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor);
void updateVerticesGlow(VertexGlow* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor, float glow);

#endif