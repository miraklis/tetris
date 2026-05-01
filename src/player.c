#include "std.h"
#include "player.h"

Player* createPlayer(int playerNum)
{
    Player* player = (Player*)malloc(sizeof(Player));
    player->playerNum = playerNum;
    player->gameSpeed = 1.0f;
    player->playerScore = 0;
    char s[20] = {0};
    sprintf(s, "Player %d :     0", playerNum);
    player->scoreText = createText(s, FONT, 22.0f, 0.0f, 0.0f);
    player->playerState = PlayerState_Playing;
    memset(player->statusMessage, 0, sizeof(player->statusMessage));
    sprintf(player->statusMessage, "%s", "Playing...\0");
    player->statusText = createText(player->statusMessage, FONT, 24.0f, 0.0f, 0.0f);
    return player;
}
