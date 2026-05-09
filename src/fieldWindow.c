
#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include <GLES3/gl3.h>
#include <unistd.h>
#include "fieldWindow.h"

void updateFieldWindow(FieldWindow* f)
{
    unsigned int strParser = 0;
    int cnt = 6; // Background never changes so we are skipping its vertices
    size_t totalChars = f->width * f->height;
    
    while(strParser < totalChars) {
        if(f->layout[strParser]=='.') {
            strParser++;
            continue;
        }        
        Color borderColor = colorGray60;
        Color vertColor = f->layout[strParser] == '*' ? colorBlack : 
                        (f->layout[strParser] == '#' ? colorGray20 : 
                        (f->layout[strParser] == '=' ? colorBlack :
                        (f->layout[strParser] == '0' ? colorRed :
                        (f->layout[strParser] == '1' ? colorBlue :
                        (f->layout[strParser] == '2' ? colorGreen :
                        (f->layout[strParser] == '3' ? colorYellow :
                        (f->layout[strParser] == '4' ? colorPink :
                        (f->layout[strParser] == '5' ? colorCyan :
                        (f->layout[strParser] == '6' ? colorPurple : colorBlack)))))))));
        float borderThickness = 1.0f;
        if(f->layout[strParser]=='*') {
            borderThickness = 0.5f;
            borderColor = colorGray10;
        }
        float glow = f->layout[strParser] == '=' ?  1.0f : 0.0f;
        updateBlockVertices(
            f->vertices, 
            &cnt,
            ((int)(strParser % f->width) * BLOCK_WIDTH),
            ((int)(strParser / f->width) * BLOCK_HEIGHT),
            borderThickness,
            &vertColor, &borderColor, glow);
        strParser++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * f->vertCount, f->vertices);

}


FieldWindow* createFieldWindow(int wx, int wy, int width, int height, bool filledBackground, Color* backgroundColor)
{
    FieldWindow* f = (FieldWindow*)malloc(sizeof(FieldWindow));

    f->wx = wx;
    f->wy = wy;
    f->width = width;
    f->height = height;
    f->rx = wx * BLOCK_WIDTH;
    f->ry = wy * BLOCK_HEIGHT;

    f->vertCount = filledBackground ? (width * height * 2) * 6 : (((width * 2) + ((height - 2) * 2)) * 2) * 6;
    f->vertCount += 6; // for the background
    f->vertices = (Vertex*)malloc(sizeof(Vertex) * f->vertCount);

    char* layout = (char*)malloc(sizeof(char) * width * height);
    char fillingChar = filledBackground ? '*' : '.';
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(j==0 || i ==0 || j==width-1 || i==height-1)
                layout[(i * width) + j] = '#';
            else
                layout[(i * width) + j] = fillingChar;
        }
    }
    f->layout = layout;

    int cnt = 0;
    // Background
    float bx = 0.0f;
    float by = 0.0f;
    float bw = width * BLOCK_WIDTH;
    float bh = height * BLOCK_HEIGHT;
    f->vertices[cnt++] = (Vertex){bx, by, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx + bw, by, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx, by + bh, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx, by + bh, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx + bw, by, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};
    f->vertices[cnt++] = (Vertex){bx + bw, by + bh, backgroundColor->r, backgroundColor->g, backgroundColor->b, backgroundColor->a, 0.0f};

    unsigned int strParser = 0;
    size_t totalChars = width * height;
    while(strParser < totalChars) {
        if(f->layout[strParser]=='.') {
            strParser++;
            continue;
        }
        Color borderColor = colorGray60;
        Color vertColor = f->layout[strParser] == '*' ? colorBlack : 
                        (f->layout[strParser] == '#' ? colorGray20 : 
                        (f->layout[strParser] == '=' ? colorWhite :
                        (f->layout[strParser] == '0' ? colorRed :
                        (f->layout[strParser] == '1' ? colorBlue :
                        (f->layout[strParser] == '2' ? colorGreen :
                        (f->layout[strParser] == '3' ? colorYellow :
                        (f->layout[strParser] == '4' ? colorPink :
                        (f->layout[strParser] == '5' ? colorCyan :
                        (f->layout[strParser] == '6' ? colorPurple : colorBlack)))))))));
        float borderThickness = 1.0f;
        if(f->layout[strParser]=='*') {
            borderThickness = 0.5f;
            borderColor = colorGray10;
        }
        updateBlockVertices(
            f->vertices, 
            &cnt,
            ((int)(strParser % width) * BLOCK_WIDTH),
            ((int)(strParser / width) * BLOCK_HEIGHT),
            borderThickness,
            &vertColor, &borderColor, 0.0f);
        strParser++;
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
    
    orthoMatrix(0, dm->w, dm->h, 0, -1, 1, f->proj);
    translateMatrix(f->rx, f->ry, f->model);

    return f;
}

void drawFieldWindow(FieldWindow *f, GameShader *shader) {
    useProgram(shader->program);
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, f->proj);
    glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, f->model);
    float tm = SDL_GetTicks() / 200.0f;
    glUniform1f(shader->locTime, tm);
    glBindVertexArray(f->vao);
    glDrawArrays(GL_TRIANGLES, 0, f->vertCount);
}

void destroyFieldWindow(FieldWindow* f)
{
    if(f==NULL)
        return;
    glDeleteVertexArrays(1, &f->vao);
    glDeleteBuffers(1, &f->vbo);
    FREE(f->vertices);
    FREE(f->layout);
    FREE(f);
}