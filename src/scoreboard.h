#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <string>
#include <vector>
#include <SDL3/SDL.h>

#define MAX_SCORES 10

typedef struct {
    std::vector<int> scores;
} ScoreBoard;

void writeToFile(ScoreBoard& sb);
void readFromFile(ScoreBoard& sb);
void addScore(ScoreBoard& sb, int score);
void drawScoreBoard(SDL_Renderer* renderer, float x, float y,ScoreBoard* sb);

#endif