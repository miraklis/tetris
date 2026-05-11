#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "shaders.h"
#include "window.h"
#include "text.h"

#define MAX_SCORES 10

typedef struct sScoreBoard {
    bool visible;
    float fontSize;

    Window* window;
    int scores[MAX_SCORES];
    Text* items[MAX_SCORES];
} ScoreBoard;

ScoreBoard* createScoreBoard(int wx, int wy, float fontSize);
void addScore(ScoreBoard* sb, int score);
void drawScoreBoard(ScoreBoard* sb,  SimpleShader* gameShader, ColoredTextureShader* coloredTextureShader);
void destroyScoreBoard(ScoreBoard* sb);

#endif