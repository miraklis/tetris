#include "std.h"
#include "graphics.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

ColorsPalette palette = {
    .colorBlack =           {0.0f, 0.0f, 0.0f, 1.0f},
    .colorGray10 =          {0.1f, 0.1f, 0.1f, 1.0f},
    .colorGray20 =          {0.2f, 0.2f, 0.2f, 1.0f},
    .colorGray60 =          {0.6f, 0.6f, 0.6f, 1.0f},
    .colorRed =             {1.0f, 0.0f, 0.0f, 1.0f},
    .colorBlue =            {0.0f, 0.0f, 1.0f, 1.0f},
    .colorGreen =           {0.0f, 1.0f, 0.0f, 1.0f},
    .colorYellow =          {1.0f, 1.0f, 0.0f, 1.0f},
    .colorPink =            {0.8f, 0.2f, 0.5f, 1.0f},
    .colorCyan =            {0.0f, 1.0f, 1.0f, 1.0f},
    .colorPurple =          {0.3f, 0.2f, 0.9f, 1.0f},
    .colorWhite =           {1.0f, 1.0f, 1.0f, 1.0f},
    .transparentBlack80 =   {0.0f, 0.0f, 0.0f, 0.8f}
};

Graphics graphics;

void initializeGraphics(void)
{
    int num_displays;
    graphics.displays = SDL_GetDisplays(&num_displays);
    graphics.dm = SDL_GetDesktopDisplayMode(graphics.displays[0]);
    
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    graphics.screenWidth = graphics.dm->w;
    graphics.screenHeight = graphics.dm->h;
    graphics.window = SDL_CreateWindow("Tetris", graphics.screenWidth, graphics.screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    graphics.context = SDL_GL_CreateContext(graphics.window);
    SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, graphics.screenWidth, graphics.screenHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

    float ratio = graphics.screenWidth / graphics.screenHeight;
    graphics.blockWidth = (uint32_t)(graphics.screenWidth / 56.0f);
    graphics.blockHeight = (uint32_t)((graphics.screenHeight / 56.0f) * ratio);

    printf("Screen: %dx%d, Block: %dx%d\n", (int)graphics.screenWidth, (int)graphics.screenHeight, graphics.blockWidth, graphics.blockHeight);

}

void destroyGraphics(void)
{
    SDL_GL_DestroyContext(graphics.context);
    SDL_DestroyWindow(graphics.window);
    SDL_free(graphics.displays);
    SDL_Quit();
}

void orthoMatrix(float left, float right, float bottom, float top, float near, float far, float* m)
{
    m[0] = 2/(right-left);  m[4] = 0;               m[8] = 0;               m[12] = -(right+left)/(right-left);
    m[1] = 0;               m[5] = 2/(top-bottom);  m[9] = 0;               m[13] = -(top+bottom)/(top-bottom);
    m[2] = 0;               m[6] = 0;               m[10] = -2/(far-near);  m[14] = -(far+near)/(far-near);
    m[3] = 0;               m[7] = 0;               m[11] = 0;              m[15] = 1;
}

void translateMatrix(float x, float y, float* m)
{
    m[0] = 1;  m[4] = 0;  m[8]  = 0;  m[12] = x;
    m[1] = 0;  m[5] = 1;  m[9]  = 0;  m[13] = y;
    m[2] = 0;  m[6] = 0;  m[10] = 1;  m[14] = 0;
    m[3] = 0;  m[7] = 0;  m[11] = 0;  m[15] = 1;
}

void rotateMatrix(float angle, float* m)
{
    angle = angle * M_PI / 180.0f;
    float c = cos(angle);
    float s = sin(angle);

    m[0] = c;   m[4] = -s;  m[8]  = 0;  m[12] = 0;
    m[1] = s;   m[5] =  c;  m[9]  = 0;  m[13] = 0;
    m[2] = 0;   m[6] =  0;  m[10] = 1;  m[14] = 0;
    m[3] = 0;   m[7] =  0;  m[11] = 0;  m[15] = 1;
}

void multiplyMatrix4x4(const float* a, const float* b, float* out)
{
    // OpenGL uses column-major matrices
    // out = a * b
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            out[col * 4 + row] =
                a[0 * 4 + row] * b[col * 4 + 0] +
                a[1 * 4 + row] * b[col * 4 + 1] +
                a[2 * 4 + row] * b[col * 4 + 2] +
                a[3 * 4 + row] * b[col * 4 + 3];
        }
    }
}

inline void updateBlockVertices(Vertex* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor, float glow)
{
        float bw = graphics.blockWidth;
        float bh = graphics.blockHeight;
        // Background block (used as border of the block)
        vertices[(*count)++] = (Vertex){bx, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (Vertex){bx + bw, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (Vertex){bx, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (Vertex){bx, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (Vertex){bx + bw, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (Vertex){bx + bw, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        // Define border thickness
        bx += borderThickness; by += borderThickness;
        bw -= (borderThickness * 2.0f); bh -= (borderThickness * 2.0f);
        // Foreground Block
        vertices[(*count)++] = (Vertex){bx, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (Vertex){bx + bw, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (Vertex){bx, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (Vertex){bx, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (Vertex){bx + bw, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (Vertex){bx + bw, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
}