#ifndef PLAYER_H
#define PLAYER_H

#include "text.h"

typedef enum {
    PlayerState_Idle = 0,
    PlayerState_Playing,
    PlayerState_GameOver,
    PlayerState_Paused,
    PlayerState_Count
} PlayerState;

typedef struct sPlayer {
    unsigned int playerScore;
    unsigned char playerNum;
    float gameSpeed;
    char statusMessage[20];
    PlayerState playerState;
} Player;

Player* createPlayer(int playerNum);

#endif // PLAYER_H