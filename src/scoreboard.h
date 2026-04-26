#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "shaders.h"
#include "tetrominoe.h"
#include "text.h"

#define MAX_SCORES 10

typedef struct sScoreBoard {
    float x, y;
    float fontSize;
    bool visible;
    int scores[MAX_SCORES];
    Text* items[MAX_SCORES];
} ScoreBoard;

ScoreBoard* createScoreBoard(float x, float y, float fontSize);
//void writeToFile(ScoreBoard* sb);
//void readFromFile(ScoreBoard* sb);
void addScore(ScoreBoard* sb, int score);
void drawScoreBoard(ScoreBoard* sb, ColoredTextureShader* shader);

#endif