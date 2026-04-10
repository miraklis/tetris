#include <cstddef>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iterator>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_hints.h>

#include "field.h"
#include "tetrominoe.h"
#include "scoreboard.h"
#include "menu.h"
#include "text.h"

#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

#define PIECE_DOWN_SCORE 25
#define LINE_FILLED_SCORE 50

enum class GameState {
    InMenu,
    Playing,
    Paused,
    GameOver,
    Count
};

// Menu callback function
void menuHandler(int selection, int* actions)
{
    switch(selection) {
        case 0:
            actions[0] = 1;
            break;
        case 1:
            actions[1] = 1;
            break;
        case 2:
            actions[2] = 1;
            break;
        case 3:
            actions[3] = 1;
            break;
    }
}

// Init the game variables
void init1PlayerGame(PlayField& f, Tetrominoe& cur, Tetrominoe& next) {
    const float fieldX = BLOCK_WIDTH;
    const float fieldY = BLOCK_HEIGHT * 5;
    unsigned int rndPieceNum;

    // Create Field
    f = createField(fieldX, fieldY);

    // Create first random piece
    std::srand(std::time({}));
    rndPieceNum = std::rand() % (TT_Count);
    cur = createTetrominoe((TetrominoeType)rndPieceNum);
    attachTetrominoeToField(&cur, &f);
    cur.isAlive = true;

    // Create and place next random piece
    std::srand(std::time({}) + 1000);
    rndPieceNum = std::rand() % (TT_Count);
    next = createTetrominoe((TetrominoeType)rndPieceNum);
    placeTetrominoe(&next, f.x, f.y - (BLOCK_HEIGHT * 4));
}

// Init the game variables
void initGame(PlayField** pField, Tetrominoe** curPiece, Tetrominoe** nextPiece, size_t numPlayers) {
    const float fieldX = BLOCK_WIDTH;
    const float fieldY = BLOCK_HEIGHT * 5;
    unsigned int rndPieceNum;

    if(*pField) {
        free(*pField);
        *pField=NULL;
    }
    if(*curPiece)
        delete[] curPiece;
    if(*nextPiece)
        delete[] nextPiece;

    PlayField* field;// = (PlayField*)malloc(sizeof(PlayField) * numPlayers);
    Tetrominoe* cur = new Tetrominoe[numPlayers];
    Tetrominoe* next = new Tetrominoe[numPlayers];

    printf("numPlayers = %zu\n", numPlayers);
    printf("field ptr = %p\n", field);
    printf("sizeof PlayField = %zu\n", sizeof(PlayField));

    float xOffset = 0.0;
    for(int i = 0; i < numPlayers; i++) {
        // Create Field
        PlayField test = createField(fieldX + xOffset, fieldY);
        field[i] = createField(fieldX + xOffset, fieldY);

        // Create first random piece
        rndPieceNum = std::rand() % (TT_Count);
        cur[i] = createTetrominoe((TetrominoeType)rndPieceNum);
        attachTetrominoeToField(&cur[i], &field[i]);
        cur[i].isAlive = true;

        // Create and place next random piece
        rndPieceNum = std::rand() % (TT_Count);
        next[i] = createTetrominoe((TetrominoeType)rndPieceNum);
        placeTetrominoe(&next[i], field[i].x, field[i].y - (BLOCK_HEIGHT * 4));
        xOffset += 600.0f;
    }
       
    *pField = field;
    *curPiece = cur;
    *nextPiece = next;
}

void readInput(SDL_Event& ev, bool keyPress[6])
{
    static const bool* currentKeyStates = SDL_GetKeyboardState(nullptr);
    static bool previousKeyStates[SDL_SCANCODE_COUNT] = {};
    static unsigned long repeatDelay[6] = {SDL_GetTicks()};

    SDL_PollEvent(&ev);
    if(ev.type == SDL_EVENT_QUIT)
        keyPress[0] = true;
    // Escape (exit game)
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
    // Preserve previous key states
    std::memcpy(previousKeyStates, currentKeyStates, SDL_SCANCODE_COUNT);
}

int main(int argc, char** argv)
{
    // ************************
    // Init SDL and Window
    // ************************
    SDL_Init(SDL_INIT_VIDEO);
    
    int num_displays;
    SDL_DisplayID *displays = SDL_GetDisplays(&num_displays);
    const SDL_DisplayMode* dm;
    dm = SDL_GetDesktopDisplayMode(displays[0]);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer("Tetris", dm->w, dm->h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer);
    int wWidth, wHeight;
    SDL_GetWindowSize(window, &wWidth, &wHeight);
    SDL_SetRenderLogicalPresentation(renderer, wWidth, wHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    std::srand(std::time(nullptr));

    // Declare and initialize variables
    struct {
        bool running = true;
        unsigned int playerScore = 0;
        float gameSpeed = 1.0f;
        GameState gameState = GameState::InMenu;
    } game;
    
    // Game objects
    PlayField* pField = nullptr;
    Tetrominoe* currentPiece = nullptr;
    Tetrominoe* nextPiece = nullptr;
    ScoreBoard scoreBoard;
    Menu menu;
    
    bool pieceOnStack = false;
    bool filledAnimation = false;
    unsigned long lastTick = 0;
    unsigned long lastStepTick = 0;
    unsigned int piecesCnt = 0;
    std::string scoreText = "";
    GameState lastState = game.gameState;
    
    // Initialize menu
    std::vector<std::string> menuItems = {
        " 1 PLAYER GAME ",
        " 2 PLAYER GAME ",
        "   EXIT GAME   "
    };
    int menuAction[4];
    menu = createMenu(menuItems, wWidth / 2.0f, wHeight / 2.0f, 3.0f, menuAction, menuHandler);

    SDL_Event ev = {};
    bool keyPress[7] = {};
    while (game.running)
    {
        unsigned long frameStart = SDL_GetTicks();

        // ************************
        // READ INPUT FROM KEYBOARD
        // ************************
        if(game.gameState == GameState::InMenu) {
            handleMenuInput(ev, menu);
        }
        else {
            readInput(ev, keyPress);
        }

        // ************************
        // UPDATE GAME
        // ************************
        // Menu Actions ***********
        if(menuAction[0] == 1) { // close menu
            game.gameState = lastState;
        }
        if(menuAction[1] == 1) { // 1 player game
            // Initialize variables
            game.gameState = GameState::Playing;
            game.gameSpeed = 1.0f;
            game.playerScore = 0;
            pieceOnStack = false;
            lastTick = SDL_GetTicks();
            lastStepTick = SDL_GetTicks();
            scoreText = "SCORE : 0";
            piecesCnt = 0;
            filledAnimation = false;
            // Create objects
            //init1PlayerGame(pField, currentPiece, nextPiece);
            initGame(&pField, &currentPiece, &nextPiece, 1);
            // GetHighScores
            readFromFile(scoreBoard);
        }
        if(menuAction[2] == 1) { // 1 player game
            // Initialize variables
            game.gameState = GameState::Playing;
            game.gameSpeed = 1.0f;
            game.playerScore = 0;
            pieceOnStack = false;
            lastTick = SDL_GetTicks();
            lastStepTick = SDL_GetTicks();
            scoreText = "SCORE : 0";
            piecesCnt = 0;
            filledAnimation = false;
            // Create objects
            //init1PlayerGame(pField, currentPiece, nextPiece);
            initGame(&pField, &currentPiece, &nextPiece, 2);
            // GetHighScores
            readFromFile(scoreBoard);
        }
        if(menuAction[3] == 1) { // exit game
            game.running = false;
        }
        SDL_zero(menuAction);
        // Game Actions ***********
        if(keyPress[0]) {
            game.running = false;
        }
        // Show Menu
        if(keyPress[1]) {
            lastState = game.gameState;
            game.gameState = GameState::InMenu;
        }
        // Pause Game
        if(keyPress[6] && game.gameState == GameState::Playing) {
            game.gameState = GameState::Paused;
            keyPress[6] = false;
        }
        // Unpause Game
        if(keyPress[6] && game.gameState == GameState::Paused) {
            game.gameState = GameState::Playing;
            keyPress[6] = false;
        }
        // While paused disable all movements
        if(game.gameState == GameState::Paused) {
            SDL_zero(keyPress);
        }
        // Move current piece left
        if(keyPress[2] && currentPiece->isAlive && !checkCollissions(currentPiece, pField, -1, 0, false)) {
            moveTetrominoe(-1, 0, currentPiece);
        }
        // Move current piece right
        if(keyPress[3] && currentPiece->isAlive && !checkCollissions(currentPiece, pField, 1, 0, false)) {
            moveTetrominoe(1, 0, currentPiece);
        }
        // Move current piece down
        if(keyPress[4] && currentPiece->isAlive) {
            if(!checkCollissions(currentPiece, pField, 0, 1, false)) {
                moveTetrominoe(0, 1, currentPiece);
            } else {
                pieceOnStack = true;
            }
        }
        // Rotate current piece
        if(keyPress[5] && currentPiece->isAlive && !checkCollissions(currentPiece, pField, 0, 0, true)) {
            rotateTetrominoe(currentPiece);
        }
        // Reset keypresses
        SDL_zero(keyPress);

        // Game Logic
        if(game.gameState == GameState::Playing) {
            // Update gameSpeed
            if(piecesCnt >= 20) {
                game.gameSpeed *= 1.5f;
                if(game.gameSpeed > 12.0f)
                    game.gameSpeed = 12.0f;
                piecesCnt = 0;
            }
            // If step time passed, move down a block
            unsigned long stepTicks = SDL_GetTicks();
            if(stepTicks - lastStepTick >= (1000.0f / game.gameSpeed)) {
                if(currentPiece->isAlive) {
                    if(!checkCollissions(currentPiece, pField, 0, 1,false)) {
                        moveTetrominoe(0, 1, currentPiece);
                        lastStepTick = stepTicks;
                    } else {
                        pieceOnStack = true;
                    }
                }
            }

            // We are showing filled lines animation
            unsigned long filledAnimationTicks;
            if(filledAnimation) {
                pieceOnStack = false;
                if(SDL_GetTicks() - filledAnimationTicks >= 120.0f) {
                    filledAnimation = false;
                    removeFilledLines(pField);
                }
            }

            // Piece hit the bottom stack
            if(pieceOnStack) {
                pieceOnStack = false;
                // Make tetrominoe a piece of the stack
                int x = currentPiece->wx;
                int y = currentPiece->wy;
                for(int i = 0; i<16; i+=4) {
                    char c = "0123456"[currentPiece->type];
                    if(currentPiece->shapeLayout[i + 0] != '.')
                        pField->fieldLayout[(y * FIELD_WIDTH) + x + 0] = c;//'0';
                    if(currentPiece->shapeLayout[i + 1] != '.')
                        pField->fieldLayout[(y * FIELD_WIDTH) + x + 1] = c;//'0';
                    if(currentPiece->shapeLayout[i + 2] != '.')
                        pField->fieldLayout[(y * FIELD_WIDTH) + x + 2] = c;//'0';                    
                    if(currentPiece->shapeLayout[i + 3] != '.')
                        pField->fieldLayout[(y * FIELD_WIDTH) + x + 3] = c;//'0';
                    y++;
                }
                updateField(pField);
                piecesCnt++;
                // Check for lines filled
                int scoreFromLines = 0;
                scoreFromLines = checkForFilledLines(pField, currentPiece);
                if(scoreFromLines > 0) {
                    game.playerScore += scoreFromLines * LINE_FILLED_SCORE;
                    filledAnimationTicks = SDL_GetTicks();
                    filledAnimation = true;
                    updateField(pField);
                } else {
                    game.playerScore += PIECE_DOWN_SCORE;
                }
                scoreText = "SCORE: " + std::to_string(game.playerScore);
                // Check for Game Over
                // std::string firstfieldLine = pField->fieldLayout.substr((3 * FIELD_WIDTH), FIELD_WIDTH);
                // if(firstfieldLine.find("0") != std::string::npos || firstfieldLine.find("1") != std::string::npos ||
                //    firstfieldLine.find("2") != std::string::npos || firstfieldLine.find("3") != std::string::npos ||
                //    firstfieldLine.find("4") != std::string::npos || firstfieldLine.find("5") != std::string::npos ||
                //    firstfieldLine.find("6") != std::string::npos) {
                if(checkGameOver(pField)) {
                        currentPiece->isAlive = false;
                        addScore(scoreBoard, game.playerScore);
                        game.gameState = GameState::GameOver;
                    }
                // Get next piece and create new one
                if(game.gameState != GameState::GameOver) {
                    // Make next piece current
                    *currentPiece = *nextPiece;
                    attachTetrominoeToField(currentPiece, pField);
                    currentPiece->isAlive = true;
                    // Create new next piece
                    unsigned int rndPieceNum = std::rand() % (TT_Count);
                    *nextPiece = createTetrominoe((TetrominoeType)rndPieceNum);
                    placeTetrominoe(nextPiece, pField->x, pField->y - (BLOCK_HEIGHT * 4));
                }
            }
        }
        
        // ************************
        // RENDER
        // ************************
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Black
        SDL_RenderClear(renderer);
        
        if(pField) {
            drawField(renderer, pField);
            //drawField(renderer, &pField[1]);
        }
        // if(currentPiece) {
        //     drawTetrominoe(renderer, &currentPiece[0]);
        //     //drawTetrominoe(renderer, &currentPiece[1]);
        // }
        // if(nextPiece) {
        //     drawTetrominoe(renderer, &nextPiece[0]);
        //     //drawTetrominoe(renderer, &nextPiece[1]);
        // }
        // drawScoreBoard(renderer, (pField->x + (FIELD_WIDTH * BLOCK_WIDTH) + 100.0f), 200.0f, &scoreBoard);
        // drawText(renderer, 
        //     scoreText, 
        //     {0.2f, 0.4f, 1.0f, 1.0f}, 
        //     (pField->x + (FIELD_WIDTH * BLOCK_WIDTH) + 100.0f), 100.0f, 3.0f);
        // if(game.gameState == GameState::GameOver) {
        //     drawText(renderer, 
        //         "GAME OVER !!!",
        //         {1.0f, 0.4f, 0.2f, 1.0f}, 
        //         (pField->x + (FIELD_WIDTH * BLOCK_WIDTH) + 100.0f), 150.0f, 3.0f);
        // }
        // if(game.gameState == GameState::Paused) {
        //     drawText(renderer, 
        //         "GAME PAUSED !",
        //         {1.0f, 0.4f, 0.2f, 1.0f}, 
        //         (pField->x + (FIELD_WIDTH * BLOCK_WIDTH) + 100.0f), 150.0f, 3.0f);
        // }
        if(game.gameState == GameState::InMenu) {
            drawMenu(renderer, &menu);
        }
        SDL_RenderPresent(renderer);

        // Calcuation for fixed FPS
        unsigned long frameTime = SDL_GetTicks() - frameStart;
        if(frameTime < FRAME_DELAY ) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // CLEANUP RESOURCES
    if(pField)
        free(pField);
    if(currentPiece)
        delete[] currentPiece;
    if(nextPiece)
        delete[] nextPiece;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // EXIT GAME (GG)
    return EXIT_SUCCESS;
}