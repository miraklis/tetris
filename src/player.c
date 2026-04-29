#include "std.h"
#include "player.h"

Player* createPlayer(int playerNum)
{
    Player* player = (Player*)malloc(sizeof(Player));
    player->playerNum = playerNum;
    player->gameSpeed = 1.0f;
    player->playerScore = 0;
    char s[20];
    sprintf(s, "Player %d :     0", playerNum);
    player->scoreText = createText(s, FONT, 22.0f, 0.0f, 0.0f);
    player->playerState = PlayerState_Playing;
    sprintf(player->statusMessage, "%s", "Playing...");
    player->statusText = createText(player->statusMessage, FONT, 24.0f, 0.0f, 0.0f);
    return player;
}
