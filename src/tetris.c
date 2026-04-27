#include <time.h>
#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "field.h"
#include "player.h"
#include "tetrominoe.h"
#include "text.h"
#include "menu.h"
#include "image.h"
#include "scoreboard.h"

#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

#define PIECE_DOWN_SCORE 25
#define LINE_FILLED_SCORE 50

#define MAX_PLAYERS 2

typedef enum {
    GameState_InMenu = 0,
    GameState_Playing,
    GameState_Paused,
    GameState_Over,
    GameState_Count
} GameState;

typedef struct Game {
    bool running;// = true;
    GameState gameState;// = GameState::GameState_InMenu;
    GameState lastState;// = GameState::GameState_InMenu;
} Game;

void freeGameObjects(PlayField* field[MAX_PLAYERS], Player* player[MAX_PLAYERS])
{
    for(int i=0; i < MAX_PLAYERS; i++) {
        if(field[i] != NULL) {
            free(field[i]);
            field[i] = NULL;
        }
        if(player[i] != NULL) {
            free(player[i]->scoreText);
            free(player[i]);
            player[i] = NULL;
        }
    }
}

void readInput(const bool* currentKeyStates, bool keyPress[11])
{
    static bool previousKeyStates[SDL_SCANCODE_COUNT] = {0};
    static unsigned long repeatDelay[11];// = {SDL_GetTicks()};

    // Escape (show menu)
    if(currentKeyStates[SDL_SCANCODE_ESCAPE] && !previousKeyStates[SDL_SCANCODE_ESCAPE]) {
        keyPress[1] = true;
    }
    // P (pause)
    if(currentKeyStates[SDL_SCANCODE_P] && !previousKeyStates[SDL_SCANCODE_P]) {
        keyPress[6] = true;
    }
    // LEFT ARROW
    if(currentKeyStates[SDL_SCANCODE_LEFT] && !previousKeyStates[SDL_SCANCODE_LEFT]) {
        keyPress[2] = true;
        repeatDelay[2] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_LEFT] && previousKeyStates[SDL_SCANCODE_LEFT]) {
        if(SDL_GetTicks() - repeatDelay[2] >= 100) {
            repeatDelay[2] = SDL_GetTicks();
            keyPress[2] = true;
        }
    }
    // RIGHT ARROW
    if(currentKeyStates[SDL_SCANCODE_RIGHT] && !previousKeyStates[SDL_SCANCODE_RIGHT]) {
        keyPress[3] = true;
        repeatDelay[3] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_RIGHT] && previousKeyStates[SDL_SCANCODE_RIGHT]) {
        if(SDL_GetTicks() - repeatDelay[3] >= 100) {
            repeatDelay[3] = SDL_GetTicks();
            keyPress[3] = true;
        }
    }
    // DOWN ARROW
    if(currentKeyStates[SDL_SCANCODE_DOWN] && !previousKeyStates[SDL_SCANCODE_DOWN]) {
        keyPress[4] = true;
        repeatDelay[4] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_DOWN] && previousKeyStates[SDL_SCANCODE_DOWN]) {
        if(SDL_GetTicks() - repeatDelay[4] >= 50) {
            repeatDelay[4] = SDL_GetTicks();
            keyPress[4] = true;
        }
    }
    // UP ARROW
    if(currentKeyStates[SDL_SCANCODE_UP] && !previousKeyStates[SDL_SCANCODE_UP]) {
        keyPress[5] = true;
        repeatDelay[5] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_UP] && previousKeyStates[SDL_SCANCODE_UP]) {
        if(SDL_GetTicks() - repeatDelay[5] >= 200) {
            repeatDelay[5] = SDL_GetTicks();
            keyPress[5] = true;
        }
    }

    // W (left)
    if(currentKeyStates[SDL_SCANCODE_A] && !previousKeyStates[SDL_SCANCODE_A]) {
        keyPress[7] = true;
        repeatDelay[7] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_A] && previousKeyStates[SDL_SCANCODE_A]) {
        if(SDL_GetTicks() - repeatDelay[7] >= 100) {
            repeatDelay[7] = SDL_GetTicks();
            keyPress[7] = true;
        }
    }
    // D (right)
    if(currentKeyStates[SDL_SCANCODE_D] && !previousKeyStates[SDL_SCANCODE_D]) {
        keyPress[8] = true;
        repeatDelay[8] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_D] && previousKeyStates[SDL_SCANCODE_D]) {
        if(SDL_GetTicks() - repeatDelay[8] >= 100) {
            repeatDelay[8] = SDL_GetTicks();
            keyPress[8] = true;
        }
    }
    // S (down)
    if(currentKeyStates[SDL_SCANCODE_S] && !previousKeyStates[SDL_SCANCODE_S]) {
        keyPress[9] = true;
        repeatDelay[9] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_S] && previousKeyStates[SDL_SCANCODE_S]) {
        if(SDL_GetTicks() - repeatDelay[9] >= 50) {
            repeatDelay[9] = SDL_GetTicks();
            keyPress[9] = true;
        }
    }
    // W (up)
    if(currentKeyStates[SDL_SCANCODE_W] && !previousKeyStates[SDL_SCANCODE_W]) {
        keyPress[10] = true;
        repeatDelay[10] = SDL_GetTicks();
    }
    if(currentKeyStates[SDL_SCANCODE_W] && previousKeyStates[SDL_SCANCODE_W]) {
        if(SDL_GetTicks() - repeatDelay[10] >= 200) {
            repeatDelay[10] = SDL_GetTicks();
            keyPress[10] = true;
        }
    }

    // Preserve previous key states
    memcpy(previousKeyStates, currentKeyStates, SDL_SCANCODE_COUNT);
}

int main(int argc, char** argv)
{
    // ************************
    // Init SDL and Window
    // ************************
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    int num_displays;
    SDL_DisplayID *displays = SDL_GetDisplays(&num_displays);
    dm = SDL_GetDesktopDisplayMode(displays[0]);
    
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    float wWidth = dm->w;
    float wHeight = dm->h;
    SDL_Window* window= SDL_CreateWindow("Tetris", wWidth, wHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // vsync
    glViewport(0, 0, wWidth, wHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

    srand(time(NULL));

    // ************************
    // Create Shaders
    // ************************    
    GameShader* gameShader = createGameShader();
    TextureShader* textureShader = createTextureShader();
    ColoredTextureShader* coloredTextureShader = createColoredTextureShader();

    // ***************************
    // Create and initialize game
    // ***************************

    Game game;
    game.gameState = game.lastState = GameState_InMenu;
    game.running = true;

    Image* splash_screen = loadImage("assets/splash_screen.jpg");

    Menu* menu;
    menu = createMenu("1 PLAYER GAME|2 PLAYER GAME|EXIT|", wWidth / 2.0f, wHeight / 2.0f, 24.0f); 

    PlayField* field[MAX_PLAYERS];
    Player* player[MAX_PLAYERS];
    for(int i=0; i < MAX_PLAYERS; i++) {
        field[i] = NULL;
        player[i] = NULL;
    }
    
    size_t sbFontSize = 22.0f;
    ScoreBoard* scoreBoard = createScoreBoard(wWidth / 2 - (sbFontSize * 12), wHeight / 2 - (sbFontSize * 5), sbFontSize);
    
    Text* gameStatusMessage = createText("GAME STARTED !!!", FONT, 48.0f, wWidth / 2.0f, wHeight / 2.0f);
    setTextColor(gameStatusMessage, 1.0f, 0.0f, 0.0f);

    bool keyPress[11] = {0};
    SDL_Event ev;
    while(game.running) {
        unsigned long frameStart = SDL_GetTicks();
        // ************************
        // READ INPUT FROM KEYBOARD
        // ************************
        SDL_PollEvent(&ev);
        const bool* currentKeyStates = SDL_GetKeyboardState(NULL);
        if(game.gameState == GameState_InMenu) {
            handleMenuInput(currentKeyStates, menu);
        }
        else {
            readInput(currentKeyStates, keyPress);
        }
        // ************************
        // Game Actions
        // ************************
        // Quit game if we close the window or select exit from the menu
        if(ev.type == SDL_EVENT_QUIT || menu->action == 3) {
            SDL_zero(keyPress);
            game.running = false;
        }
        // Hide menu
        if(menu->action == 0 && game.gameState == GameState_InMenu && game.lastState != GameState_InMenu) {
            game.gameState = game.lastState;
            menu->visible = false;
        }
        // Show menu
        if(keyPress[1] && game.gameState != GameState_InMenu) {
            game.lastState = game.gameState;
            game.gameState = GameState_InMenu;
            menu->visible = true;
        }
        // Start 1 Player game
        if(menu->action == 1) { // 1 player game
            freeGameObjects(field, player);
            field[0] = createField(1, 0);
            createNewTetrominoes(field[0]);
            player[0] = createPlayer(1);
            player[0]->scoreText = createText(
                "Player 1 :     0", FONT,
                22.0f, field[0]->rx + (FIELD_WIDTH * BLOCK_WIDTH), field[0]->ry + 44.0f
            );
            game.gameState = GameState_Playing;
            menu->visible = false;
            splash_screen->visible = false;
            scoreBoard->visible = true;

        }
        // Start 1 Player game
        if(menu->action == 2) { // 1 player game
            freeGameObjects(field, player);
            field[1] = createField(1, 0);
            createNewTetrominoes(field[1]);
            player[1] = createPlayer(2);
            player[1]->scoreText = createText(
                "Player 2 :     0", FONT,
                22.0f, field[1]->rx + (FIELD_WIDTH * BLOCK_WIDTH), field[1]->ry + 44.0f
            );
            field[0] = createField((int)(dm->w / BLOCK_WIDTH) - FIELD_WIDTH, 0);
            createNewTetrominoes(field[0]);
            placeTetrominoe(field[0]->wx - 4, FIELD_START_ROW, field[0]->nextPiece);
            player[0] = createPlayer(1);
            player[0]->scoreText = createText(
                "Player 1 :     0", FONT,
                22.0f, field[0]->rx - (22.0f * 22.0f), field[0]->ry + 44.0f
            );

            game.gameState = GameState_Playing;
            menu->visible = false;
            splash_screen->visible = false;
            scoreBoard->visible = true;
        }
        menu->action = -1;
        // Game Actions
        // While in menu or paused disable all movements
        if(game.gameState == GameState_InMenu || game.gameState == GameState_Paused) {
            SDL_zero(keyPress);
        }
        // Pieces Movements
        // Player 1
        if(player[0] == NULL || player[0]->playerState != PlayerState_Playing) {
            keyPress[2] = keyPress[3] = keyPress[4] = keyPress[5] = false;
        }
        if(keyPress[2]) {
            moveCurrentPiece(field[0], -1, 0);
        }
        if(keyPress[3]) {
            moveCurrentPiece(field[0], 1, 0);
        }
        if(keyPress[4]) {
            moveCurrentPiece(field[0], 0, 1);
            if(checkCollissions(field[0], field[0]->currentPiece, 0, 1,false))
                field[0]->pieceOnStack = true;
        }
        if(keyPress[5]) {
            rotateCurrentPiece(field[0]);
        }
        // Player 2
        if(player[1] == NULL || player[1]->playerState != PlayerState_Playing) {
            keyPress[7] = keyPress[8] = keyPress[9] = keyPress[10] = false;
        }
        if(keyPress[7]) {
            moveCurrentPiece(field[1], -1, 0);
        }
        if(keyPress[8]) {
            moveCurrentPiece(field[1], 1, 0);
        }
        if(keyPress[9]) {
            moveCurrentPiece(field[1], 0, 1);
            if(checkCollissions(field[1], field[1]->currentPiece, 0, 1,false))
                field[1]->pieceOnStack = true;
        }
        if(keyPress[10]) {
            rotateCurrentPiece(field[1]);
        }        
        // Reset keypresses
        SDL_zero(keyPress);

        // ************************
        // Game Logic
        // ************************
        if(game.gameState == GameState_Playing) {
            for(int i=0; i < MAX_PLAYERS; i++) {
                if(player[i] == NULL)
                    continue;
                // Update gameSpeed
                if(field[i]->piecesCnt >= 20) {
                    player[i]->gameSpeed *= 1.5f;
                    if(player[i]->gameSpeed > 12.0f)
                        player[i]->gameSpeed = 12.0f;
                    field[i]->piecesCnt = 0;
                }
                // If step time passed, move down a block
                unsigned long stepTicks = SDL_GetTicks();
                if(stepTicks - field[i]->lastStepTick >= (1000.0f / player[i]->gameSpeed)) {
                    if(field[i]->currentPiece->isAlive) {
                        if(!checkCollissions(field[i], field[i]->currentPiece, 0, 1,false)) {
                            moveTetrominoe(0, 1, field[i]->currentPiece);
                            field[i]->lastStepTick = stepTicks;
                        } else {
                            field[i]->pieceOnStack = true;
                        }
                    }
                }
                if(field[i]->pieceOnStack) {
                    field[i]->pieceOnStack = false;
                    // Make tetrominoe a piece of the stack
                    addPieceOnStack(field[i], field[i]->currentPiece);
                    // Check for lines filled
                    int scoreFromLines = 0;
                    scoreFromLines = checkForFilledLines(field[i], field[i]->currentPiece);
                    if(scoreFromLines > 0) {
                        player[i]->playerScore += scoreFromLines * LINE_FILLED_SCORE;
                        removeFilledLines(field[i]);
                    } else {
                        player[i]->playerScore += PIECE_DOWN_SCORE;
                    }
                    char scoreStr[20];
                    sprintf(scoreStr, "%6d", player[i]->playerScore);
                    changeText(player[i]->scoreText, 11, scoreStr);
                    // Check for Game Over
                    if(checkGameOver(field[i])) {
                        field[i]->currentPiece->isAlive = false;
                        player[i]->playerState = PlayerState_GameOver;
                        addScore(scoreBoard, player[i]->playerScore);
                    }
                    // Get next piece and create new one
                    if(player[i]->playerState != PlayerState_GameOver) {
                        getNextTetrominoe(field[i]);
                    }
                }
            }
            if(player[0] != NULL && player[0]->playerState == PlayerState_GameOver
                && player[1] != NULL && player[1]->playerState == PlayerState_GameOver) {
                    game.gameState = GameState_Over;
                    game.lastState = GameState_Over;
            }
        }

        // ************************
        // RENDER
        // ************************
        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawImage(splash_screen, textureShader);
        
        int cnt = 0;
        while(field[cnt] != NULL && cnt < MAX_PLAYERS) {
            drawField(field[cnt], gameShader);
            cnt++;
        }

        cnt = 0;
        while(player[cnt] != NULL && player[cnt]->scoreText != NULL && cnt < MAX_PLAYERS) {
            drawText(player[cnt]->scoreText, coloredTextureShader);
            cnt++;
        }

        drawScoreBoard(scoreBoard, coloredTextureShader);
        drawMenu(menu, gameShader, coloredTextureShader);

        SDL_GL_SwapWindow(window);

        // Calcuation for fixed FPS
        unsigned long frameTime = SDL_GetTicks() - frameStart;
        if(frameTime < FRAME_DELAY ) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // Free GameObjects
    freeGameObjects(field, player);   
    free(splash_screen);
    free(menu);
    free(scoreBoard);

    // Cleanup
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_free(displays);
    SDL_Quit();

    return 0;
}
