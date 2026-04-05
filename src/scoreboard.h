#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <string>
#include <vector>
#include <SDL3/SDL.h>

#define MAX_SCORES 10

typedef struct {
    std::string text;
    SDL_Color color;
} HighScoreText;

void updateScoreBoard(HighScoreText* sb, std::vector<int> scores);
void writeToFile(std::vector<int>& highScores);
void readFromFile(std::vector<int>& highScores);

#endif