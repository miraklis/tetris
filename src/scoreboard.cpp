#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>

#include "scoreboard.h"

static const char* highScoresFile = "highscores.txt";

// Write to highscore file
void writeToFile(ScoreBoard& sb)
{
    std::ofstream fs;
    fs.open(highScoresFile);
    if(!fs.is_open()) {
        return;
    }

    for(auto& score : sb.scores) {
        fs << score << std::endl;
    }

    fs.flush();
    fs.close();
}

// Read from highscore file
void readFromFile(ScoreBoard& sb)
{
    std::ifstream fs;
    fs.open(highScoresFile);
    if(!fs.is_open()) {
        sb.scores = std::vector<int>(MAX_SCORES, 0);
        writeToFile(sb);
        return;
    }

    int val = 0;
    sb.scores.clear();// = std::vector<int>(MAX_SCORES, 0);
    while(fs >> val) {
        sb.scores.push_back(val);
    }
    std::sort(sb.scores.begin(), sb.scores.end(), [](int a, int b) { return a > b; });
    fs.close();
}

void addScore(ScoreBoard& sb, int score)
{
    if(score < sb.scores[MAX_SCORES - 1])
        return;

    for(int i=0; i < sb.scores.size(); i++) {
        if(score >= sb.scores[i]) {
            for(int j = sb.scores.size() - 1; j > i; j--) {
                sb.scores[j] = sb.scores[j-1];
            }
            sb.scores[i] = score;
            writeToFile(sb);
            break;
        }
    }

}
void drawScoreBoard(SDL_Renderer* renderer, float x, float y, ScoreBoard* sb)
{
    if(!sb)
        return;
    std::ostringstream ss;
    float scale = 3.0f;
    SDL_SetRenderScale(renderer, scale, scale);
    for(int i = 0; i < sb->scores.size(); i++) {
        ss.str("");
        ss << std::setw(8) << sb->scores[i];
        if(i < 3) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 75, 175, 125, 255);
        }
        SDL_RenderDebugText(renderer, (x / scale), (y / scale) + (i * 10), ss.str().c_str());
    }
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}