#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include <GLES3/gl3.h>
#include "tetrominoe.h"

static void updateModelMatrix(Tetrominoe* t)
{
    float translation[16];
    float rotation[16];
    translateMatrix(
        t->fx + ((t->wx + t->centerOffsetX) * BLOCK_WIDTH),
        t->fy + ((t->wy + t->centerOffsetY) * BLOCK_HEIGHT),
        translation
    );
    rotateMatrix(t->rotationState * 90.0f, rotation);
    multiplyMatrix4x4(translation, rotation, t->model);
}

Tetrominoe* createTetrominoe(TetrominoeType type)
{
    Tetrominoe* t = (Tetrominoe*)malloc(sizeof(Tetrominoe));

    t->type = type;
    t->isAlive = false;
    t->wx = 0;
    t->wy = 0;
    t->fx = 0;
    t->fy = 0;
    t->rotationState = 0;

    // Define layout and color
    switch(t->type) {
        case TT_Line:
        // ..X.
        // ..X.
        // ..X.
        // ..X.
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X...X...X...X.");
            t->color = colorRed;
            break;
        case TT_Square:
        // ....
        // .XX.
        // .XX.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", ".....XX..XX.....");
            t->color = colorBlue;
            break;
        case TT_T:
        // ..X.
        // .XX.
        // ..X.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X..XX...X.....");
            t->color = colorGreen;
            break;
        case TT_L1:
        // .X..
        // .X..
        // .XX.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", ".X...X...XX.....");
            t->color = colorYellow;
            break;
        case TT_L2:
        // ..X.
        // ..X.
        // .XX.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X...X..XX.....");
            t->color = colorPink;
            break;
        case TT_h1:
        // .X..
        // .XX.
        // ..X.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", ".X...XX...X.....");
            t->color = colorCyan;
            break;
        case TT_h2:
        // ..X.
        // .XX.
        // .X..
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X..XX..X......");
            t->color = colorPurple;
            break;
        default:
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "................");
            t->color = colorBlack;
            break;        
    }
    // move the origin to the center of the piece for easier rotation
    t->centerOffsetX = 2.0f;
    t->centerOffsetY = 2.0f;
    // Create vertices (4 blocks * 12 vertices per block = 48 vertices total)
    unsigned int strParser = 0;
    int vertCnt = 0;
    while(strParser < sizeof(t->shapeLayout) - 1) {
        if(t->shapeLayout[strParser]=='.') {
            strParser++;
            continue;
        } else {
            Color wc = {0.6f, 0.6f, 0.6f, 1.0f};
            float borderThickness = 1.0f;
            updateBlockVertices(
                t->vertices, 
                &vertCnt,
                ((int)(strParser % 4) - t->centerOffsetX) * BLOCK_WIDTH,
                ((int)(strParser / 4) - t->centerOffsetY) * BLOCK_HEIGHT,
                borderThickness,
                &t->color, &wc, 0.0f);
            // Get next character
            strParser++;
        }
    }

    // Create and update Vertex Array Buffer
    glGenVertexArrays(1, &t->vao);
    glGenBuffers(1, &t->vbo);
    glBindVertexArray(t->vao);
    glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBufferData(GL_ARRAY_BUFFER, TETROMINOE_VERTICES_COUNT * sizeof(Vertex), t->vertices, GL_DYNAMIC_DRAW);

    // Update uniforms proj and model
    orthoMatrix(0, dm->w, dm->h, 0, -1, 1, t->proj);
    updateModelMatrix(t);

    return t;
}

void placeTetrominoe(int x, int y, Tetrominoe* t)
{
    t->wx = x;
    t->wy = y;
    updateModelMatrix(t);
}

void moveTetrominoe(int x, int y, Tetrominoe* t)
{
    t->wx += x;
    t->wy += y;
    updateModelMatrix(t);
}

void rotateTetrominoe(Tetrominoe* t)
{
    t->rotationState = (t->rotationState + 1) % 4;
    updateModelMatrix(t);
    rotateLayout(t->shapeLayout);
}

void drawTetrominoe(Tetrominoe* t, GameShader* shader)//GLint program)
{
    if(t == NULL)
        return;
    useProgram(shader->program);
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, t->proj);
    glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, t->model);
    glUniform1f(shader->locTime, 0.0f);
    glBindVertexArray(t->vao);
    glDrawArrays(GL_TRIANGLES, 0, TETROMINOE_VERTICES_COUNT);
}

void rotateLayout(char layout[17]) {
    char s[17];
    snprintf(s, sizeof(s), "%s", layout);

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            layout[(i * 4) + j] = s[12 + i - (j * 4)]; // rotate 90 degrees CW
            //(*layout)[(i * 4) + j] = s[(15 - i) - ((3 - j) * 4)]; // rotate 90 degrees CCW
}