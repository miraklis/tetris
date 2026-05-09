
#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include <stdint.h>
#include "window.h"

Window* createWindow(int wx, int wy, int width, int height, Color* backgroundColor, bool hasBorder)
{
    Window* f = (Window*)malloc(sizeof(Window));

    uint32_t blockWidth = graphics.blockWidth;
    uint32_t blockHeight = graphics.blockHeight;

    f->base.id = OBJ_TYPE_Window;
    // f->base.x = wx * BLOCK_WIDTH;
    // f->base.y = wy * BLOCK_HEIGHT;
    // f->base.width = width * BLOCK_WIDTH;
    // f->base.height = height * BLOCK_HEIGHT;
    f->base.x = wx * blockWidth;
    f->base.y = wy * blockHeight;
    f->base.width = width * blockWidth;
    f->base.height = height * blockHeight;

    f->wx = wx;
    f->wy = wy;
    f->width = width;
    f->height = height;

    f->vertCount = hasBorder ? ((((width * 2) + ((height - 2) * 2)) * 2) * 6) + 6 : 6; // 6 for the background
    f->vertices = (Vertex*)malloc(sizeof(Vertex) * f->vertCount);

    int cnt = 0;
    // Background
    float bx = 0.0f;
    float by = 0.0f;
    float bw = width * blockWidth;
    float bh = height * blockHeight;
    f->vertices[cnt++] = (Vertex){bx, by, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx + bw, by, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx, by + bh, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx, by + bh, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx + bw, by, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx + bw, by + bh, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};

    if(hasBorder) {
        for(int i=0; i < height; i++) {
            for(int j=0; j < width; j++) {
                if(i==0 || j==0 || i==height-1 || j==width-1) {
                    if(i==0 || j==0 || i==height-1 || j==width-1) {
                        updateBlockVertices(
                            f->vertices, 
                            &cnt,
                            (j * blockWidth),
                            (i * blockHeight),
                            1.0f,
                            &palette.colorGray20, &palette.colorGray60, 0.0f);
                    }
                }
            }
        }
    }

    glGenVertexArrays(1, &f->vao);
    glGenBuffers(1, &f->vbo);
    glBindVertexArray(f->vao);
    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, f->vertCount * sizeof(Vertex), f->vertices, GL_DYNAMIC_DRAW);
    
    orthoMatrix(0, graphics.screenWidth, graphics.screenHeight, 0, -1, 1, f->proj);
    translateMatrix(f->base.x, f->base.y, f->model);

    return f;
}

void addObjectToWindow(Window* f, Object* o)
{
    o->x = f->base.x;
    o->y = f->base.y;
}

void drawWindow(Window *f, GameShader *shader) {
    useProgram(shader->program);
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, f->proj);
    glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, f->model);
    glUniform1f(shader->locTime, 0.0f);
    glBindVertexArray(f->vao);
    glDrawArrays(GL_TRIANGLES, 0, f->vertCount);
}

void destroyWindow(Window* f)
{
    if(f==NULL)
        return;
    glDeleteVertexArrays(1, &f->vao);
    glDeleteBuffers(1, &f->vbo);
    FREE(f->vertices);
    FREE(f);
}