#include <fstream>
#include <iomanip>
#include <algorithm>

#include "scoreboard.h"

static const char* highScoresFile = "highscores.txt";

// Update the scoreboard
void updateScoreBoard(HighScoreText* sb, std::vector<int> scores) {
    for(int i = 0; i < 10; i++) {
        std::ostringstream ss;
        ss.str("");
        ss << std::setw(8) << scores[i];
        sb->text = ss.str();
        if(i < 3) {
            sb->color = {255, 0, 0, 255};
        } else {
            sb->color = {75, 175, 125, 255};
        }
        sb++;
    }
}

// Write to highscore file
void writeToFile(std::vector<int>& highScores)
{
    std::ofstream fs;
    fs.open(highScoresFile);
    if(!fs.is_open()) {
        return;
    }

    for(auto& hi : highScores) {
        fs << hi << std::endl;
    }
    fs.flush();
    fs.close();
}

// Read from highscore file
void readFromFile(std::vector<int>& highScores)
{
    std::ifstream fs;
    fs.open(highScoresFile);
    if(!fs.is_open()) {
        highScores = std::vector<int>(10 , 0);
        writeToFile(highScores);
        return;
    }

    int i = 0;
    int val = 0;
    highScores.clear();
    while(fs >> val) {
        highScores.push_back(val);
    }
    std::sort(highScores.begin(), highScores.end(), [](int a, int b) { return a > b; });
    fs.close();
}
