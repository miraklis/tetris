#ifndef PLAYER_H
#define PLAYER_H

#include "text.h"

typedef enum {
    PlayerState_Idle = 0,
    PlayerState_Playing,
    PlayerState_GameOver,
    PlayerState_Count
} PlayerState;

typedef struct sPlayer {
    unsigned int playerScore;
    float gameSpeed;
    char playerNum;
    char statusMessage[20];
    PlayerState playerState;
    Text* scoreText;
    Text* statusText;
} Player;

Player* createPlayer(int playerNum);

#endif // PLAYER_H