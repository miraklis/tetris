#ifndef FIELD_H
#define FIELD_H

#include "graphics.h"
#include "shaders.h"
#include "tetrominoe.h"
#include "player.h"

typedef struct sPlayField {
    int wx, wy;
    float rx, ry;
    char fieldLayout[FIELD_WIDTH * FIELD_HEIGHT];

    unsigned long lastTick;
    unsigned long lastStepTick;
    unsigned int piecesCnt;
    bool pieceOnStack;

    size_t vertCount;
    Tetrominoe* currentPiece;
    Tetrominoe* nextPiece;

    GLuint vao;
    GLuint vbo;
    float proj[16];
    float model[16];
    Vertex* vertices;
} PlayField;

PlayField* createField(int wx, int wy);
void createNewTetrominoes(PlayField* f);
void getNextTetrominoe(PlayField* f);
void moveCurrentPiece(PlayField* f, int x, int y);
void rotateCurrentPiece(PlayField* f);

void drawField(PlayField* f, GameShader* shader);
void updateField(PlayField* f);
void removeFilledLines(PlayField* f);
int checkForFilledLines(PlayField* f, Tetrominoe* t);
bool checkGameOver(PlayField* f);
bool checkCollissions(PlayField* f, Tetrominoe* t, int offsetX, int offsetY, bool rotated);
void addPieceOnStack(PlayField* f, Tetrominoe* t);

#endif // FIELD_H