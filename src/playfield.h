#ifndef FIELD_H
#define FIELD_H

#include "graphics.h"
#include "shaders.h"
#include "window.h"
#include "arena.h"
#include "tetrominoe.h"
#include "player.h"

typedef struct sPlayField {
    Window* playWindow;
    Window* nextPieceWindow;
    Window* infoWindow;
    Arena* arena;
    Tetrominoe* currentPiece;
    Tetrominoe* nextPiece;
    Player* player;
    Text* scoreText;
    Text* statusText;

    unsigned long lastTick;
    unsigned long lastStepTick;
    unsigned long lastGlowTick;
    unsigned int piecesCnt;
    bool pieceOnStack;
    bool glowEffect;
} PlayField;


PlayField* createField(int wx, int wy, unsigned char playernum, bool nextWindowPosToLeft);
void getNextTetrominoe(PlayField* f);
void moveCurrentPiece(PlayField* f, int x, int y);
void rotateCurrentPiece(PlayField* f);

void drawField(PlayField* f, SimpleShader* shader,  GlowShader* glowShader, ColoredTextureShader* coloredTextureShader);
void removeFilledLines(PlayField* f);
int checkForFilledLines(PlayField* f, Tetrominoe* t);
bool checkGameOver(PlayField* f);
bool checkCollissions(PlayField* f, Tetrominoe* t, int offsetX, int offsetY, bool rotated);
void addPieceOnStack(PlayField* f, Tetrominoe* t);

#endif // FIELD_H