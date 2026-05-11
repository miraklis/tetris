#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "arena.h"

void updateArena(Arena* f)
{
    unsigned int strParser = 0;
    int cnt = 0;
    size_t totalChars = f->width * f->height;

    uint32_t blockWidth = graphics.blockWidth;
    uint32_t blockHeight = graphics.blockHeight;    

    while(strParser < totalChars) {
        Color borderColor = palette.colorGray60;
        Color vertColor = f->layout[strParser] == '*' ? palette.colorBlack : 
                        (f->layout[strParser] == '#' ? palette.colorGray20 : 
                        (f->layout[strParser] == '=' ? palette.colorBlack :
                        (f->layout[strParser] == '0' ? palette.colorRed :
                        (f->layout[strParser] == '1' ? palette.colorBlue :
                        (f->layout[strParser] == '2' ? palette.colorGreen :
                        (f->layout[strParser] == '3' ? palette.colorYellow :
                        (f->layout[strParser] == '4' ? palette.colorPink :
                        (f->layout[strParser] == '5' ? palette.colorCyan :
                        (f->layout[strParser] == '6' ? palette.colorPurple : palette.colorBlack)))))))));
        float borderThickness = 1.0f;
        if(f->layout[strParser]=='*') {
            borderThickness = 0.5f;
            borderColor = palette.colorGray10;
        }
        float glow = f->layout[strParser] == '=' ?  1.0f : 0.0f;
        updateVerticesGlow(
            f->vertices, 
            &cnt,
            ((int)(strParser % f->width) * blockWidth),
            ((int)(strParser / f->width) * blockHeight),
            borderThickness,
            &vertColor, &borderColor, glow);
        strParser++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexGlow) * f->vertCount, f->vertices);

}


Arena* createArena(int wx, int wy, int width, int height)
{
    Arena* f = (Arena*)malloc(sizeof(Arena));

    uint32_t blockWidth = graphics.blockWidth;
    uint32_t blockHeight = graphics.blockHeight;     

    f->base.id = OBJ_TYPE_Arena;
    f->base.x = wx * blockWidth;
    f->base.y = wy * blockHeight;
    f->base.width = width * blockWidth;
    f->base.height = height * blockHeight;
    f->wx = wx;
    f->wy = wy;
    f->width = width;
    f->height = height;

    f->vertCount = (width * height * 2) * 6;
    f->vertices = (VertexGlow*)malloc(sizeof(VertexGlow) * f->vertCount);

    char* layout = (char*)malloc(sizeof(char) * width * height);
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(j==0 || i ==0 || j==width-1 || i==height-1)
                layout[(i * width) + j] = '#';
            else
                layout[(i * width) + j] = '*';
        }
    }
    f->layout = layout;

    int cnt = 0;
    unsigned int strParser = 0;
    size_t totalChars = width * height;
    while(strParser < totalChars) {
        Color borderColor = palette.colorGray60;
        Color vertColor = f->layout[strParser] == '*' ? palette.colorBlack : 
                        (f->layout[strParser] == '#' ? palette.colorGray20 : 
                        (f->layout[strParser] == '=' ? palette.colorWhite :
                        (f->layout[strParser] == '0' ? palette.colorRed :
                        (f->layout[strParser] == '1' ? palette.colorBlue :
                        (f->layout[strParser] == '2' ? palette.colorGreen :
                        (f->layout[strParser] == '3' ? palette.colorYellow :
                        (f->layout[strParser] == '4' ? palette.colorPink :
                        (f->layout[strParser] == '5' ? palette.colorCyan :
                        (f->layout[strParser] == '6' ? palette.colorPurple : palette.colorBlack)))))))));
        float borderThickness = 1.0f;
        if(f->layout[strParser]=='*') {
            borderThickness = 0.5f;
            borderColor = palette.colorGray10;
        }
        updateVerticesGlow(
            f->vertices, 
            &cnt,
            ((int)(strParser % width) * blockWidth),
            ((int)(strParser / width) * blockHeight),
            borderThickness,
            &vertColor, &borderColor, 0.0f);
        strParser++;
    }

    glGenVertexArrays(1, &f->vao);
    glGenBuffers(1, &f->vbo);
    setupVertexLayout(f->vao, f->vbo, VAO_LAYOUT_GLOW);
    // glBindVertexArray(f->vao);
    // glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6*sizeof(float)));
    // glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, f->vertCount * sizeof(VertexGlow), f->vertices, GL_DYNAMIC_DRAW);
    
    orthoMatrix(0, graphics.screenWidth, graphics.screenHeight, 0, -1, 1, f->proj);
    translateMatrix(f->base.x, f->base.y, f->model);

    return f;
}

void drawArena(Arena *f, GlowShader *shader) {
    useProgram(shader->program);
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, f->proj);
    glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, f->model);
    float tm = SDL_GetTicks() / 200.0f;
    glUniform1f(shader->locTime, tm);
    glBindVertexArray(f->vao);
    glDrawArrays(GL_TRIANGLES, 0, f->vertCount);
}

void destroyArena(Arena* f)
{
    if(f==NULL)
        return;
    glDeleteVertexArrays(1, &f->vao);
    glDeleteBuffers(1, &f->vbo);
    FREE(f->vertices);
    FREE(f->layout);
    FREE(f);
}