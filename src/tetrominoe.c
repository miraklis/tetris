#include "std.h"
#include "graphics.h"
#include "shaders.h"
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
            t->color = (Color){1.0f, 0.0f, 0.0f, 1.0f};
            break;
        case TT_Square:
        // ....
        // .XX.
        // .XX.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", ".....XX..XX.....");
            t->color = (Color){0.0f, 0.0f, 1.0f, 1.0f};
            break;
        case TT_T:
        // ..X.
        // .XX.
        // ..X.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X..XX...X.....");
            t->color = (Color){0.0f, 1.0f, 0.0f, 1.0f};
            break;
        case TT_L1:
        // .X..
        // .X..
        // .XX.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", ".X...X...XX.....");
            t->color = (Color){1.0f, 1.0f, 0.0f, 1.0f};
            break;
        case TT_L2:
        // ..X.
        // ..X.
        // .XX.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X...X..XX.....");
            t->color = (Color){0.8f, 0.2f, 0.5f, 1.0f};
            break;
        case TT_h1:
        // .X..
        // .XX.
        // ..X.
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", ".X...XX...X.....");
            t->color = (Color){0.0f, 1.0f, 1.0f, 1.0f};
            break;
        case TT_h2:
        // ..X.
        // .XX.
        // .X..
        // ....
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "..X..XX..X......");
            t->color = (Color){0.3f, 0.2f, 0.9f, 1.0f};
            break;
        default:
            snprintf(t->shapeLayout, sizeof(t->shapeLayout), "%s", "................");
            t->color = (Color){0.0f, 0.0f, 0.0f, 0.0f};
            break;        
    }
    // move the origin to the center of the piece for easier rotation
    t->centerOffsetX = 2.0f;
    t->centerOffsetY = 2.0f;
    // Create vertices (4 blocks * 12 vertices per block = 48 vertices total)
    unsigned int strParser = 0;
    unsigned int vertCnt = 0;
    while(strParser < sizeof(t->shapeLayout) - 1) {
        if(t->shapeLayout[strParser]=='.') {
            strParser++;
            continue;
        } else {
            float bw = BLOCK_WIDTH;
            float bh = BLOCK_HEIGHT;            
            float bx = ((int)(strParser % 4) - t->centerOffsetX) * bw;
            float by = ((int)(strParser / 4) - t->centerOffsetY) * bh;        
            // Background block
            Color wc = {0.6f, 0.6f, 0.6f, 1.0f};
            t->vertices[vertCnt++] = (Vertex){bx, by, wc.r, wc.g, wc.b, wc.a};
            t->vertices[vertCnt++] = (Vertex){bx + bw, by, wc.r, wc.g, wc.b, wc.a};
            t->vertices[vertCnt++] = (Vertex){bx, by + bh, wc.r, wc.g, wc.b, wc.a};
            t->vertices[vertCnt++] = (Vertex){bx, by + bh, wc.r, wc.g, wc.b, wc.a};
            t->vertices[vertCnt++] = (Vertex){bx + bw, by, wc.r, wc.g, wc.b, wc.a}; 
            t->vertices[vertCnt++] = (Vertex){bx + bw, by + bh, wc.r, wc.g, wc.b, wc.a};
            bx += 1.0f; by += 1.0f;
            bw -= 2.0f; bh -= 2.0f;
            // Foreground Block
            t->vertices[vertCnt++] = (Vertex){bx, by, t->color.r, t->color.g, t->color.b, t->color.a};
            t->vertices[vertCnt++] = (Vertex){bx + bw, by, t->color.r, t->color.g, t->color.b, t->color.a};
            t->vertices[vertCnt++] = (Vertex){bx, by + bh, t->color.r, t->color.g, t->color.b, t->color.a};
            t->vertices[vertCnt++] = (Vertex){bx, by + bh, t->color.r, t->color.g, t->color.b, t->color.a};
            t->vertices[vertCnt++] = (Vertex){bx + bw, by, t->color.r, t->color.g, t->color.b, t->color.a};
            t->vertices[vertCnt++] = (Vertex){bx + bw, by + bh, t->color.r, t->color.g, t->color.b, t->color.a};
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