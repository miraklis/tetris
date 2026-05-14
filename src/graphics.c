#include "std.h"
#include "shaders.h"
#include "text.h"
#include "window.h"
#include "arena.h"
#include "tetrominoe.h"
#include "menu.h"
#include "image.h"
#include <GLES3/gl3.h>
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
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

    graphics.renderContext = createRenderContex();

}

void destroyGraphics(void)
{
    destroyRenderContext(graphics.renderContext);
    SDL_GL_DestroyContext(graphics.context);
    SDL_DestroyWindow(graphics.window);
    SDL_free(graphics.displays);
    SDL_Quit();
}

RenderContext* createRenderContex(void)
{
    RenderContext* ctx = (RenderContext*)malloc(sizeof(RenderContext));

    ctx->lastProgram = 0;

    ctx->simpleShader = createSimpleShader();
    ctx->glowShader = createGlowShader();
    ctx->textureShader = createTextureShader();
    ctx->coloredTextureShader = createColoredTextureShader();

    ctx->queueCount = 0;
    ctx->queueCapacity = 64;
    ctx->queue = (Renderable*)malloc(sizeof(Renderable) * ctx->queueCapacity);

    orthoMatrix(0, graphics.screenWidth, graphics.screenHeight, 0, -1, 1, ctx->projection);

    return ctx;
}

void renderContextBeginFrame(RenderContext* ctx)
{
    ctx->lastProgram = 0;
    ctx->queueCount = 0;
}

void renderContextQueueOject(RenderContext* ctx, RenderableType type, void* data)
{
    if(ctx->queueCount >= ctx->queueCapacity) {
        ctx->queueCapacity *= 2;
        ctx->queue = (Renderable*)realloc(ctx->queue, ctx->queueCapacity);
    }

    ctx->queue[ctx->queueCount++] = (Renderable) {
        .data = data,
        .type = type
    };
}

static int compareRenderables(const void* a, const void* b)
{
    Renderable* ra = (Renderable*)a;
    Renderable* rb = (Renderable*)b;

    int orderA = ra->type == RENDERABLE_IMAGE ? 0 : 
                 ra->type == RENDERABLE_WINDOW ? 1 : 
                 ra->type == RENDERABLE_ARENA ? 2 :
                 ra->type == RENDERABLE_TETROMINOE ? 3 :
                 ra->type == RENDERABLE_MENU ? 4 :
                 ra->type == RENDERABLE_TEXT ? 5 : 6;
    int orderB = rb->type == RENDERABLE_IMAGE ? 0 : 
                 rb->type == RENDERABLE_WINDOW ? 1 : 
                 rb->type == RENDERABLE_ARENA ? 2 :
                 rb->type == RENDERABLE_TETROMINOE ? 3 :
                 rb->type == RENDERABLE_MENU ? 4 :
                 rb->type == RENDERABLE_TEXT ? 5 : 6;
    return orderA - orderB;
}

void renderContextFlush(RenderContext* ctx)
{
    if(ctx->queueCount == 0)
        return;

    qsort(ctx->queue, ctx->queueCount, sizeof(Renderable), compareRenderables);

    for(uint32_t i=0; i < ctx->queueCount; i++) {
        Renderable* r = &ctx->queue[i];

        switch(r->type) {
            case RENDERABLE_TEXT: {
                Text* t = (Text*)r->data;
                ColoredTextureShader* shader = ctx->coloredTextureShader;
                if(ctx->lastProgram != shader->program) {
                    ctx->lastProgram = shader->program;
                    glUseProgram(ctx->lastProgram);
                }
                glBindVertexArray(t->vao);
                glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, ctx->projection);
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, t->model);
                glUniform3f(shader->locColor, t->color.r, t->color.g, t->color.b);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, t->font->texture);
                glUniform1i(shader->locTexture, 0);
                glDrawArrays(GL_TRIANGLES, 0, t->vertCount);
                break;
            }
            case RENDERABLE_WINDOW: {
                Window* w = (Window*)r->data;
                SimpleShader* shader = ctx->simpleShader;
                if(ctx->lastProgram != shader->program) {
                    ctx->lastProgram = shader->program;
                    glUseProgram(ctx->lastProgram);
                }
                glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, ctx->projection);
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, w->model);
                glBindVertexArray(w->vao);
                glDrawArrays(GL_TRIANGLES, 0, w->vertCount);                
                break;
            }
            case RENDERABLE_ARENA: {
                Arena* a = (Arena*)r->data;
                GlowShader* shader = ctx->glowShader;
                if(ctx->lastProgram != shader->program) {
                    ctx->lastProgram = shader->program;
                    glUseProgram(ctx->lastProgram);
                }
                glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, ctx->projection);
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, a->model);
                float tm = SDL_GetTicks() / 200.0f;
                glUniform1f(shader->locTime, tm);
                glBindVertexArray(a->vao);
                glDrawArrays(GL_TRIANGLES, 0, a->vertCount);
                break;
            }
            case RENDERABLE_TETROMINOE: {
                Tetrominoe* t = (Tetrominoe*)r->data;
                SimpleShader* shader = ctx->simpleShader;
                if(ctx->lastProgram != shader->program) {
                    ctx->lastProgram = shader->program;
                    glUseProgram(ctx->lastProgram);
                }
                glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, ctx->projection);
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, t->model);
                glBindVertexArray(t->vao);
                glDrawArrays(GL_TRIANGLES, 0, TETROMINOE_VERTICES_COUNT);                
                break;
            }
            case RENDERABLE_MENU: {
                Menu* m = (Menu*)r->data;
                SimpleShader* shader = ctx->simpleShader;
                if(ctx->lastProgram != shader->program) {
                    ctx->lastProgram = shader->program;
                    glUseProgram(ctx->lastProgram);
                }
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, m->menuModel);
                glBindVertexArray(m->backVao);
                glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES);
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, m->barModel);
                glBindVertexArray(m->barVao);
                glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES);
                break;
            }
            case RENDERABLE_IMAGE: {
                Image* img = (Image*)r->data;
                TextureShader* shader = ctx->textureShader;
                if(ctx->lastProgram != shader->program) {
                    ctx->lastProgram = shader->program;
                    glUseProgram(ctx->lastProgram);
                }
                glBindVertexArray(img->vao);
                glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, img->proj);
                glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, img->model);
                glBindTexture(GL_TEXTURE_2D, img->textureID); 
                glActiveTexture(GL_TEXTURE0);
                glUniform1i(shader->locTexture, 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                break;
            }
            default:
                break;
        }
    }
}

void destroyRenderContext(RenderContext* ctx)
{
    destroySimpleShader(ctx->simpleShader);
    destroyGlowShader(ctx->glowShader);
    destroyTextureShader(ctx->textureShader);
    destroyColoredTextureShader(ctx->coloredTextureShader);
    FREE(ctx->queue);
    FREE(ctx);
}

void setupVertexLayout(GLuint vao, GLuint vbo, VertexLayout layout)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    switch(layout) {
        case VAO_LAYOUT_SIMPLE:
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSimple), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSimple), (void*)(2*sizeof(float)));
            glEnableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            break;
        case VAO_LAYOUT_GLOW:
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGlow), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexGlow), (void*)(2*sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(VertexGlow), (void*)(6*sizeof(float)));
            glEnableVertexAttribArray(2);
            break;
        case VAO_LAYOUT_GLYPH:
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGlyph), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexGlyph), (void*)(2*sizeof(float)));
            glEnableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            break;
        default:
            printf("No VAO layout selected to setup");
            break;
    }
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

void updateVerticesSimple(VertexSimple* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor)
{
        float bw = graphics.blockWidth;
        float bh = graphics.blockHeight;
        // Background block (used as border of the block)
        vertices[(*count)++] = (VertexSimple){bx, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a};
        vertices[(*count)++] = (VertexSimple){bx + bw, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a};
        vertices[(*count)++] = (VertexSimple){bx, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a};
        vertices[(*count)++] = (VertexSimple){bx, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a};
        vertices[(*count)++] = (VertexSimple){bx + bw, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a};
        vertices[(*count)++] = (VertexSimple){bx + bw, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a};
        // Define border thickness
        bx += borderThickness; by += borderThickness;
        bw -= (borderThickness * 2.0f); bh -= (borderThickness * 2.0f);
        // Foregrouck
        vertices[(*count)++] = (VertexSimple){bx, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a};
        vertices[(*count)++] = (VertexSimple){bx + bw, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a};
        vertices[(*count)++] = (VertexSimple){bx, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a};
        vertices[(*count)++] = (VertexSimple){bx, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a};
        vertices[(*count)++] = (VertexSimple){bx + bw, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a};
        vertices[(*count)++] = (VertexSimple){bx + bw, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a};
}

void updateVerticesGlow(VertexGlow* vertices, int* count, float bx, float by, float borderThickness, const Color* blockColor, const Color* borderColor, float glow)
{
        float bw = graphics.blockWidth;
        float bh = graphics.blockHeight;
        // Background block (used as border of the block)
        vertices[(*count)++] = (VertexGlow){bx, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (VertexGlow){bx + bw, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (VertexGlow){bx, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (VertexGlow){bx, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (VertexGlow){bx + bw, by, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        vertices[(*count)++] = (VertexGlow){bx + bw, by + bh, borderColor->r, borderColor->g, borderColor->b, borderColor->a, 0.0f};
        // Define border thickness
        bx += borderThickness; by += borderThickness;
        bw -= (borderThickness * 2.0f); bh -= (borderThickness * 2.0f);
        // Foreground Block
        vertices[(*count)++] = (VertexGlow){bx, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (VertexGlow){bx + bw, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (VertexGlow){bx, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (VertexGlow){bx, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (VertexGlow){bx + bw, by, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
        vertices[(*count)++] = (VertexGlow){bx + bw, by + bh, blockColor->r, blockColor->g, blockColor->b, blockColor->a, glow};
}