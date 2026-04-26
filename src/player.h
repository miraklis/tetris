#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
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
    PlayerState playerState;
    Text* scoreText;
} Player;

Player* createPlayer(int playerNum);

#endif // PLAYER_H