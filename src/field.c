#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "tetrominoe.h"
#include <GLES3/gl3.h>
#include <string.h>
#include "field.h"

static void updateModelMatrix(PlayField* f)
{
    translateMatrix(f->rx, f->ry, f->model);
}

static void updateField(PlayField* f, bool upload)
{
    unsigned int strParser = 0;
    int cnt = 0;
    size_t totalChars = strlen(f->fieldLayout);
    
    while(strParser < totalChars) {
        Color borderColor = colorGray60;
        Color vertColor = f->fieldLayout[strParser] == '.' ? colorBlack : 
                        (f->fieldLayout[strParser] == '#' ? colorGray20 : 
                        (f->fieldLayout[strParser] == '=' ? colorWhite :
                        (f->fieldLayout[strParser] == '0' ? colorRed :
                        (f->fieldLayout[strParser] == '1' ? colorBlue :
                        (f->fieldLayout[strParser] == '2' ? colorGreen :
                        (f->fieldLayout[strParser] == '3' ? colorYellow :
                        (f->fieldLayout[strParser] == '4' ? colorPink :
                        (f->fieldLayout[strParser] == '5' ? colorCyan :
                        (f->fieldLayout[strParser] == '6' ? colorPurple : colorBlack)))))))));
        float borderThickness = 1.0f;
        if(f->fieldLayout[strParser]=='.') {
            borderThickness = 0.5f;
            borderColor = colorGray10;
        }
        updateBlockVertices(
            f->vertices, 
            &cnt,
            ((int)(strParser % FIELD_WIDTH) * BLOCK_WIDTH),
            ((int)(strParser / FIELD_WIDTH) * BLOCK_HEIGHT),
            borderThickness,
            &vertColor, &borderColor);
        // Get next character
        strParser++;
    }

    if(upload) {
        glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * f->vertCount, f->vertices);
    }
}

PlayField* createField(int wx, int wy)
{
    PlayField* f = (PlayField*)malloc(sizeof(PlayField));

    f->vertCount = ((FIELD_HEIGHT * FIELD_HEIGHT * 2) * 6);
    f->vertices = (Vertex*)malloc(sizeof(Vertex) * f->vertCount);

    f->lastTick = 0;
    f->lastStepTick = 0;
    f->piecesCnt = 0;
    f->pieceOnStack = false;

    for(int i=0; i < FIELD_HEIGHT; i++) {
        for(int j=0; j < FIELD_WIDTH; j++) {
            if(j==0 || i ==0 || j==FIELD_WIDTH-1 || i==FIELD_HEIGHT-1)
                f->fieldLayout[(i * FIELD_WIDTH) + j] = '#';
            else
                f->fieldLayout[(i * FIELD_WIDTH) + j] = '.';;
        }
    }
    f->wx = 0;
    f->wy = 0;
    f->rx = wx * BLOCK_WIDTH;
    f->ry = wy * BLOCK_HEIGHT;

    updateField(f, false);

    glGenVertexArrays(1, &f->vao);
    glGenBuffers(1, &f->vbo);
    glBindVertexArray(f->vao);
    glBindBuffer(GL_ARRAY_BUFFER, f->vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, f->vertCount * sizeof(Vertex), f->vertices, GL_DYNAMIC_DRAW);
    
    orthoMatrix(0, dm->w, dm->h, 0, -1, 1, f->proj);
    updateModelMatrix(f);
    
    return f;
}

void attachTetrominoeToField(PlayField* f, Tetrominoe* t)
{
    t->fx = f->rx;
    t->fy = f->ry;
}

void createNewTetrominoes(PlayField* f)
{
    Tetrominoe* current = createTetrominoe((TetrominoeType)rand() % TT_Count);
    Tetrominoe* next = createTetrominoe((TetrominoeType)rand() % TT_Count);
    attachTetrominoeToField(f, current);
    attachTetrominoeToField(f, next);
    placeTetrominoe((FIELD_WIDTH / 2) - 2, 0, current);
    placeTetrominoe(FIELD_WIDTH, 0, next);
    current->isAlive = true;
    f->currentPiece = current;
    f->nextPiece = next;
}

void getNextTetrominoe(PlayField* f)
{
    int prevX = f->nextPiece->wx;
    int prevY = f->nextPiece->wy;
    free(f->currentPiece);
    f->currentPiece = f->nextPiece;
    f->currentPiece->isAlive = true;
    placeTetrominoe((FIELD_WIDTH / 2) - 2, 0, f->currentPiece);
    f->nextPiece = createTetrominoe((TetrominoeType)rand() % TT_Count);
    attachTetrominoeToField(f, f->nextPiece);
    placeTetrominoe(prevX, prevY, f->nextPiece);
}

void moveCurrentPiece(PlayField* f, int x, int y)
{
    if(f->currentPiece == NULL || !f->currentPiece->isAlive)
        return;
    if(!checkCollissions(f, f->currentPiece, x, y, false))
        moveTetrominoe(x, y, f->currentPiece);
}

void rotateCurrentPiece(PlayField* f)
{
    if(f->currentPiece == NULL || !f->currentPiece->isAlive)
        return;    
    if(!checkCollissions(f, f->currentPiece, 0, 0, true))
        rotateTetrominoe(f->currentPiece);
}

void drawField(PlayField* f, GameShader* shader)
{
    useProgram(shader->program);

    // Draw Field
    glUniformMatrix4fv(shader->locProj, 1, GL_FALSE, f->proj);
    glUniformMatrix4fv(shader->locModel, 1, GL_FALSE, f->model);
    glBindVertexArray(f->vao);
    glDrawArrays(GL_TRIANGLES, 0, f->vertCount);

    // Draw Tetrominoes
    if(f->currentPiece != NULL)
        drawTetrominoe(f->currentPiece, shader);
    if(f->nextPiece != NULL)
        drawTetrominoe(f->nextPiece, shader);
}

// Check for filled lines (flashing '=') and remove them
void removeFilledLines(PlayField* f) 
{
    int bottom = FIELD_HEIGHT - 1;
    char emptyLine[FIELD_WIDTH - 2];
    char filledLine[FIELD_WIDTH - 2];

    for(int i=0; i < FIELD_WIDTH - 2; i++) {
        emptyLine[i] = '.';
        filledLine[i] = '=';        
    }
    while(bottom > 0) {
        bottom--;
        bool filled = true;
        for(int i = 1; i < FIELD_WIDTH - 1; i++) {
            if(f->fieldLayout[(bottom * FIELD_WIDTH) + i] != '=') {
                filled = false;
                break;
            }
        }
        if(filled) {
            int yy = bottom;
            while(yy > 1) {
                for(int i = 1; i < FIELD_WIDTH - 1; i++) {
                    f->fieldLayout[(yy * FIELD_WIDTH) + i] = f->fieldLayout[((yy - 1) * FIELD_WIDTH) + i];
                }
                yy--;
            }
            updateField(f, true);
            bottom++;
        }
    }
    for(int i = 0; i < FIELD_WIDTH; i++) {
        f->fieldLayout[i] = '#';
    }
}

// Check if there is filled lines and respond with score from lines
int checkForFilledLines(PlayField* f, Tetrominoe* t)
{
    int result = 0;
    int top = t->wy;
    int bottom = (t->wy + 4) > FIELD_HEIGHT - 1? FIELD_HEIGHT - 1 : (t->wy + 4);

    while(bottom > 0 && bottom >= top) {
        bottom--;
        bool filled = true;
        for(int i = 1; i < FIELD_WIDTH - 1; i++) {
            if(f->fieldLayout[(bottom * FIELD_WIDTH) + i] == '.' 
                || f->fieldLayout[(bottom * FIELD_WIDTH) + i] == '#') {
                    filled = false;
                    break;
            }
        }
        if(filled) {
            result++;
            for(int i = 1; i < FIELD_WIDTH - 1; i++) {
                f->fieldLayout[(bottom * FIELD_WIDTH) + i] = '=';
            }
        }
    }
    if(result == 4)
      result += 4;
    if(result == 3)
        result += 2;
    if(result == 2)
        result++;
    return result;
}

bool checkGameOver(PlayField* f)
{
    bool isOver = false;
    int startRow = 0;
    for(int i = 1; i < FIELD_WIDTH - 1; i++) {
        if(f->fieldLayout[(startRow * FIELD_WIDTH) + i] != '#') {
            isOver = true;
            break;
        }
    }
    return isOver;
}

// Check for collissions between current shape and field
// The field includes and the stacked pieces
bool checkCollissions(PlayField* f, Tetrominoe* t, int offsetX, int offsetY, bool rotated)
{
    int fx = t->wx + offsetX;
    int sx = t->wx + offsetX;
    int y = t->wy + offsetY;

    // Prepare the field mask
    char fieldMask[17];
    if(fx < 0) {
        fx = 0;
    }
    if(fx > FIELD_WIDTH - 4) {
        fx = FIELD_WIDTH - 4;
    }

    int cnt = 0;
    while(y < FIELD_HEIGHT && cnt < 4) {
        for(int i = 0; i < 4; i++)
            fieldMask[(cnt * 4) + i] = f->fieldLayout[(y * FIELD_WIDTH) + fx + i];
        cnt++;
        y++;
    }

    // Prepare the shape mask    
    char shapeMask[17];
    snprintf(shapeMask, sizeof(shapeMask), "%s", t->shapeLayout);
    if(rotated)
        rotateLayout(shapeMask);

    // In case shape's mask is outside the left/right field limits
    // update the columns of the masks to be checked
    int sc = 0;
    int ec = 4;
    int foffset = 0;
    if(sx < 0) {
        sc = -sx;
        ec = 4;
        foffset = sx;
    }
    if(sx > FIELD_WIDTH -4) {
        sc = 0;
        ec = FIELD_WIDTH - sx;
        foffset = 4 - (FIELD_WIDTH - sx);
    }
    // Iterate the two masks to check for collission
    for(int r=0; r < 4; r++) {
        for(int c=sc; c < ec; c++) {
            if(shapeMask[(r * 4) + c] == 'X' && fieldMask[(r * 4) + c + foffset] != '.') {
                if(rotated) {
                    // Make the piece move by 1 and check again
                    Tetrominoe rt = *t;
                    rotateLayout(rt.shapeLayout);
                    int step = c >= 2 ? -1 : 1;
                    if(!checkCollissions(f, &rt, step, 0, false)) {
                        moveTetrominoe(step, 0, t);
                        return false;
                    } else {
                        return true;
                    }
                } else {
                    return true; // There is a collission
                }
            }
        }
    }
    return false; // No collissions
}

void addPieceOnStack(PlayField* f, Tetrominoe* t)
{
    // Make tetrominoe a piece of the stack
    int x = t->wx;
    int y = t->wy;
    for(int tc = 0; tc<16; tc+=4) {
        char c = "0123456"[t->type];
        if(t->shapeLayout[tc + 0] != '.')
            f->fieldLayout[(y * FIELD_WIDTH) + x + 0] = c;
        if(t->shapeLayout[tc + 1] != '.')
            f->fieldLayout[(y * FIELD_WIDTH) + x + 1] = c;
        if(t->shapeLayout[tc + 2] != '.')
            f->fieldLayout[(y * FIELD_WIDTH) + x + 2] = c;
        if(t->shapeLayout[tc + 3] != '.')
            f->fieldLayout[(y * FIELD_WIDTH) + x + 3] = c;
        y++;
    }
    f->piecesCnt++;
    updateField(f, true);
}