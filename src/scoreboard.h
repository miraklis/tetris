#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "fieldWindow.h"
#include "shaders.h"
#include "text.h"

#define MAX_SCORES 10

typedef struct sScoreBoard {
    bool visible;
    float fontSize;

    FieldWindow* window;
    int scores[MAX_SCORES];
    Text* items[MAX_SCORES];
} ScoreBoard;

//ScoreBoard* createScoreBoard(float x, float y, float fontSize);
ScoreBoard* createScoreBoard(int wx, int wy, float fontSize);
void addScore(ScoreBoard* sb, int score);
void drawScoreBoard(ScoreBoard* sb,  GameShader* gameShader, ColoredTextureShader* coloredTextureShader);
void destroyScoreBoard(ScoreBoard* sb);

#endif