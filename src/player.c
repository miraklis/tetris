#include "std.h"
#include "player.h"

Player* createPlayer(int playerNum)
{
    Player* player = (Player*)malloc(sizeof(Player));
    player->playerNum = playerNum;
    player->gameSpeed = 1.0f;
    player->playerScore = 0;
    player->playerState = PlayerState_Playing;

    return player;
}
