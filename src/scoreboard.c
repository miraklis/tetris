#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "shaders.h"
#include "text.h"
#include "scoreboard.h"

static const char* highScoresFile = "highscores.dat";

static int compareScores(const void* a, const void* b)
{
    return (*(int*)b - *(int*)a);
}

// Write to highscore file
static void writeToFile(ScoreBoard* sb)
{
    if(sb == NULL)
        return;

    qsort(sb->scores, MAX_SCORES, sizeof(sb->scores[0]), (int(*)(const void*, const void*))compareScores);
    FILE* f = fopen(highScoresFile, "wb");
    fwrite(sb->scores, sizeof(sb->scores[0]), MAX_SCORES, f);
    fclose(f);

}

// Read from highscore file
static void readFromFile(ScoreBoard* sb)
{
    if(sb == NULL)
        return;

    // If the file does not exist, create it with default scores
    if(access(highScoresFile, F_OK) == -1) {
        for(int i=0; i < MAX_SCORES; i++) {
            sb->scores[i] = 0;
        }
        writeToFile(sb);
        return;
    }

    FILE* f = fopen(highScoresFile, "rb");
    fread(sb->scores, sizeof(sb->scores[0]), MAX_SCORES, f);
    fclose(f);
}

void addScore(ScoreBoard* sb, int score)
{
    if(sb == NULL)
        return;
    if(score < sb->scores[MAX_SCORES - 1])
        return;

    sb->scores[MAX_SCORES - 1] = score;
    qsort(sb->scores, MAX_SCORES, sizeof(sb->scores[0]), (int(*)(const void*, const void*))compareScores);
    
    for(int i=0; i < MAX_SCORES; i++) {
        char scoreText[11];
        snprintf(scoreText, sizeof(scoreText), "%6d", sb->scores[i]);
        changeText(sb->items[i], 12, scoreText);
    }
    
    writeToFile(sb);
}

ScoreBoard* createScoreBoard(float x, float y, float fontSize)
{
    ScoreBoard* sb = (ScoreBoard*)malloc(sizeof(ScoreBoard));
    sb->x = x;
    sb->y = y;
    sb->fontSize = fontSize;
    sb->visible = false;
    
    readFromFile(sb);

    char* positions[MAX_SCORES] = {
        "1ST PLACE :",
        "2ND PLACE :",
        "3RD PLACE :",
        "4TH PLACE :",
        "5TH PLACE :",
        "6TH PLACE :",
        "7TH PLACE :",
        "8TH PLACE :",
        "9TH PLACE :",
        "10TH PLACE:"
    };

    for(int i = 0; i < MAX_SCORES; i++) {
        char scoreText[22];
        snprintf(scoreText, sizeof(scoreText), "%s %6d", positions[i], sb->scores[i]);
        sb->items[i] = (Text*)malloc(sizeof(Text));
        initText(
            sb->items[i], 
            scoreText,
            FONT,
            sb->fontSize,
            sb->x,
            sb->y + (i * sb->fontSize * 1.5f)
        );
        if(i < 3) {
            setTextColor(sb->items[i], 1.0f, 0.0f, 0.0f);
        }
    }

    return sb;
}

void drawScoreBoard(ScoreBoard* sb, ColoredTextureShader* shader)
{
    if(sb == NULL || !sb->visible)
        return;

    for(int i = 0; i < MAX_SCORES; i++) {
        if(sb->items[i] != NULL) {
            drawText(sb->items[i], shader);
        }
    }
}
