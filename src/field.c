#include "fieldWindow.h"
#include "player.h"
#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "tetrominoe.h"
#include <GLES3/gl3.h>
#include <string.h>
#include "field.h"

static const int startPosX = (FIELD_WIDTH / 2) - 2;
static const int startPosY = 0;

static void attachTetrominoeToWindow(FieldWindow* f, Tetrominoe* t)
{
    t->fx = f->rx;
    t->fy = f->ry;
}

static void createNewTetrominoes(PlayField* f)
{
    Tetrominoe* current = createTetrominoe((TetrominoeType)rand() % TT_Count);
    Tetrominoe* next = createTetrominoe((TetrominoeType)rand() % TT_Count);
    attachTetrominoeToWindow(f->fieldWindow, current);
    attachTetrominoeToWindow(f->nextPieceWindow, next);
    placeTetrominoe(startPosX, startPosY, current);
    placeTetrominoe(1, 1, next);
    current->isAlive = true;
    f->currentPiece = current;
    f->nextPiece = next;
}

PlayField* createField(int wx, int wy, unsigned char playerNum, bool nextWindowPosToLeft)
{
    PlayField* f = (PlayField*)malloc(sizeof(PlayField));
    f->fieldWindow = createFieldWindow(wx, wy, FIELD_WIDTH, FIELD_HEIGHT, true, &colorBlack);
    int nextWindowX = nextWindowPosToLeft ? (wx - 6) : (wx + FIELD_WIDTH);
    f->nextPieceWindow = createFieldWindow(nextWindowX, wy, 6, 6, false, &colorBlack);
    f->infoWindow = createFieldWindow(wx, wy + FIELD_HEIGHT, FIELD_WIDTH, 6, false, &colorBlack);
    f->player = createPlayer(playerNum);
    f->player->playerState = PlayerState_Playing;

    
    float paddingX = BLOCK_WIDTH * 2.0f;
    float paddingY = BLOCK_HEIGHT * 2.0f;
    const float fontSize = 22.0f;

    f->scoreText = createText("Score :     0", FONT, fontSize, 0.0f, 0.0f);
    moveText(f->scoreText, f->fieldWindow->rx + paddingX, f->fieldWindow->ry + (FIELD_HEIGHT * BLOCK_HEIGHT) + paddingY);
    
    memset(f->player->statusMessage, 0, sizeof(f->player->statusMessage));
    sprintf(f->player->statusMessage, "%s", "Playing...");
    f->statusText = createText(f->player->statusMessage, FONT, fontSize, 0.0f, 0.0f);   
    moveText(f->statusText, f->scoreText->x, f->scoreText->y + (fontSize * 2.0f));
    
    createNewTetrominoes(f);

    f->lastTick = 0;
    f->lastStepTick = 0;
    f->lastGlowTick = 0;
    f->piecesCnt = 0;
    f->pieceOnStack = false;
    f->glowEffect = false;

    return f;
}

void getNextTetrominoe(PlayField* f)
{
    int prevX = f->nextPiece->wx;
    int prevY = f->nextPiece->wy;
    //free(f->currentPiece);
    destroyTetrominoe(f->currentPiece);
    f->currentPiece = f->nextPiece;
    attachTetrominoeToWindow(f->fieldWindow, f->currentPiece);
    f->currentPiece->isAlive = true;
    placeTetrominoe(startPosX, startPosY, f->currentPiece);
    f->nextPiece = createTetrominoe((TetrominoeType)rand() % TT_Count);
    attachTetrominoeToWindow(f->nextPieceWindow, f->nextPiece);
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

void drawField(PlayField* f, GameShader* shader, ColoredTextureShader* coloredTextureShader)
{
    // Draw Windows
    drawFieldWindow(f->fieldWindow, shader);
    drawFieldWindow(f->nextPieceWindow, shader);
    drawFieldWindow(f->infoWindow, shader);    
    // Draw Tetrominoes
    drawTetrominoe(f->currentPiece, shader);
    drawTetrominoe(f->nextPiece, shader);
    // Draw Texts
    drawText(f->scoreText, coloredTextureShader);
    drawText(f->statusText, coloredTextureShader);
}

// Check for filled lines (flashing '=') and remove them
void removeFilledLines(PlayField* f) 
{
    int bottom = FIELD_HEIGHT - 1;
    char emptyLine[FIELD_WIDTH - 2];
    char filledLine[FIELD_WIDTH - 2];

    for(int i=0; i < FIELD_WIDTH - 2; i++) {
        emptyLine[i] = '*';
        filledLine[i] = '=';        
    }
    while(bottom > 0) {
        bottom--;
        bool filled = true;
        for(int i = 1; i < FIELD_WIDTH - 1; i++) {
            if(f->fieldWindow->layout[(bottom * FIELD_WIDTH) + i] != '=') {
                filled = false;
                break;
            }
        }
        if(filled) {
            int yy = bottom;
            while(yy > 1) {
                for(int i = 1; i < FIELD_WIDTH - 1; i++) {
                    f->fieldWindow->layout[(yy * FIELD_WIDTH) + i] = f->fieldWindow->layout[((yy - 1) * FIELD_WIDTH) + i];
                }
                yy--;
            }
            updateFieldWindow(f->fieldWindow);
            bottom++;
        }
    }
    for(int i = 0; i < FIELD_WIDTH; i++) {
        f->fieldWindow->layout[i] = '#';
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
            if(f->fieldWindow->layout[(bottom * FIELD_WIDTH) + i] == '*' 
                || f->fieldWindow->layout[(bottom * FIELD_WIDTH) + i] == '#') {
                    filled = false;
                    break;
            }
        }
        if(filled) {
            result++;
            for(int i = 1; i < FIELD_WIDTH - 1; i++) {
                f->fieldWindow->layout[(bottom * FIELD_WIDTH) + i] = '=';
            }
        }
    }
    updateFieldWindow(f->fieldWindow);
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
        if(f->fieldWindow->layout[(startRow * FIELD_WIDTH) + i] != '#') {
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
            fieldMask[(cnt * 4) + i] = f->fieldWindow->layout[(y * FIELD_WIDTH) + fx + i];
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
            if(shapeMask[(r * 4) + c] == 'X' && fieldMask[(r * 4) + c + foffset] != '*') {
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
            f->fieldWindow->layout[(y * FIELD_WIDTH) + x + 0] = c;
        if(t->shapeLayout[tc + 1] != '.')
            f->fieldWindow->layout[(y * FIELD_WIDTH) + x + 1] = c;
        if(t->shapeLayout[tc + 2] != '.')
            f->fieldWindow->layout[(y * FIELD_WIDTH) + x + 2] = c;
        if(t->shapeLayout[tc + 3] != '.')
            f->fieldWindow->layout[(y * FIELD_WIDTH) + x + 3] = c;
        y++;
    }
    f->piecesCnt++;
    //updateFieldWindow(f->fieldWindow);
}