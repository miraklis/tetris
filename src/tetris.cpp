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

#include "field.h"
#include "tetrominoe.h"
#include "scoreboard.h"

#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define PIECE_DOWN_SCORE 25
#define LINE_FILLED_SCORE 50

// Variables
static bool running = false;
static bool render = false;
static unsigned int playerScore = 0;
static bool gameOver = false;
static bool pieceOnStack = false;
static float gameSpeed = 1.0f;
static unsigned long lastTick;
static unsigned long lastStepTick;
static std::string scoreText = "";
static unsigned int piecesCnt = 0;
static bool filledAnimation = false;
static ScoreBoard scoreBoard;

// Init the game variables
void initGame(PlayField& f, Tetrominoe& cur, Tetrominoe& next) {
    const float fieldX = 20.0f;
    const float fieldY = 100.0f;
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
    std::srand(std::time({}));
    rndPieceNum = std::rand() % (TT_Count);
    next = createTetrominoe((TetrominoeType)rndPieceNum);
    placeTetrominoe(&next, f.x, f.y - (BLOCK_HEIGHT * 4));

    // Initialize variables
    running = true;
    render = true;
    gameOver = false;
    pieceOnStack = false;
    gameSpeed = 1.0f;
    lastTick = SDL_GetTicks();
    lastStepTick = SDL_GetTicks();
    playerScore = 0;
    scoreText = "SCORE: 0";
    piecesCnt = 0;
    filledAnimation = false;

    // GetHighScores
    readFromFile(scoreBoard);
}

void readInput(bool keyPress[6])
{
    static SDL_Event ev = {};
    static const bool* currentKeyStates = SDL_GetKeyboardState(nullptr);
    static bool previousKeyStates[SDL_SCANCODE_COUNT] = {};
    static unsigned long repeatDelay[6] = {SDL_GetTicks()};

    SDL_PollEvent(&ev);
    if(ev.type == SDL_EVENT_QUIT)
        keyPress[0] = true;
    // Escape (exit game)
    if(currentKeyStates[SDL_SCANCODE_ESCAPE]) {
        keyPress[0] = true;
    }
    // F12 (new game)
    if(currentKeyStates[SDL_SCANCODE_F12]) {
        keyPress[1] = true;
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
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer("Tetris", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    PlayField pField;
    Tetrominoe currentPiece;
    Tetrominoe nextPiece;

    initGame(pField, currentPiece, nextPiece);

    bool keyPress[6] = {};
    while (running)
    {
        unsigned long frameStart = SDL_GetTicks();

        // ************************
        // READ INPUT FROM KEYBOARD
        // ************************
        readInput(keyPress);

        // ************************
        // UPDATE GAME
        // ************************
        // Exit Game
        if(keyPress[0])
            running = false;
        // New Game
        if(keyPress[1])
            initGame(pField, currentPiece, nextPiece);
        // Move current piece left
        if(keyPress[2] && currentPiece.isAlive && !checkCollissions(&currentPiece, &pField, -1, 0, false)) {
            moveTetrominoe(-1, 0, &currentPiece);
            render = true;
        }
        // Move current piece right
        if(keyPress[3] && currentPiece.isAlive && !checkCollissions(&currentPiece, &pField, 1, 0, false)) {
            moveTetrominoe(1, 0, &currentPiece);
            render = true;
        }
        // Move current piece down
        if(keyPress[4] && currentPiece.isAlive) {
            if(!checkCollissions(&currentPiece, &pField, 0, 1, false)) {
                moveTetrominoe(0, 1, &currentPiece);
                render = true;
            } else {
                pieceOnStack = true;
            }
        }
        // Rotate current piece
        if(keyPress[5] && currentPiece.isAlive && !checkCollissions(&currentPiece, &pField, 0, 0, true)) {
            rotateTetrominoe(&currentPiece);
            render = true;
        }
        // Reset keypresses
        SDL_zero(keyPress);

        // Game Logic
        if(!gameOver) {
            // Update gameSpeed
            if(piecesCnt >= 20) {
                gameSpeed *= 1.5f;
                if(gameSpeed > 20.0f)
                    gameSpeed = 20.0f;
                piecesCnt = 0;
            }
            // If step time passed, move down a block
            unsigned long stepTicks = SDL_GetTicks();
            if(stepTicks - lastStepTick >= (1000.0f / gameSpeed)) {
                if(currentPiece.isAlive) {
                    if(!checkCollissions(&currentPiece, &pField, 0, 1,false)) {
                        moveTetrominoe(0, 1, &currentPiece);
                        lastStepTick = stepTicks;
                    } else {
                        pieceOnStack = true;
                    }
                    render = true;
                }
            }

            // We are showing filled lines animation
            unsigned long filledAnimationTicks;
            if(filledAnimation) {
                render = true;
                //currentPiece.isAlive = false;
                pieceOnStack = false;
                if(SDL_GetTicks() - filledAnimationTicks >= 120.0f) {
                    filledAnimation = false;
                    removeFilledLines(&pField);
                    currentPiece.isAlive = true;
                }
            }

            // Piece hit the bottom stack
            if(pieceOnStack) {
                pieceOnStack = false;
                render = true;
                // Make tetrominoe a piece of the stack
                int x = currentPiece.wx;
                int y = currentPiece.wy;
                for(int i = 0; i<16; i+=4) {
                    char c = "0123456"[currentPiece.type];
                    if(currentPiece.shapeLayout[i + 0] != '.')
                        pField.fieldLayout[(y * FIELD_WIDTH) + x + 0] = c;//'0';
                    if(currentPiece.shapeLayout[i + 1] != '.')
                        pField.fieldLayout[(y * FIELD_WIDTH) + x + 1] = c;//'0';
                    if(currentPiece.shapeLayout[i + 2] != '.')
                        pField.fieldLayout[(y * FIELD_WIDTH) + x + 2] = c;//'0';                    
                    if(currentPiece.shapeLayout[i + 3] != '.')
                        pField.fieldLayout[(y * FIELD_WIDTH) + x + 3] = c;//'0';
                    y++;
                }
                updateField(&pField);
                piecesCnt++;
                // Check for lines filled
                int scoreFromLines = 0;
                scoreFromLines = checkForFilledLines(&pField, &currentPiece);
                if(scoreFromLines > 0) {
                    playerScore += scoreFromLines * LINE_FILLED_SCORE;
                    filledAnimationTicks = SDL_GetTicks();
                    filledAnimation = true;
                    updateField(&pField);
                } else {
                    playerScore += PIECE_DOWN_SCORE;
                }
                scoreText = "SCORE: " + std::to_string(playerScore);
                // Check for Game Over
                std::string firstfieldLine = pField.fieldLayout.substr((3 * FIELD_WIDTH), FIELD_WIDTH);
                if(firstfieldLine.find("0") != std::string::npos || firstfieldLine.find("1") != std::string::npos ||
                   firstfieldLine.find("2") != std::string::npos || firstfieldLine.find("3") != std::string::npos ||
                   firstfieldLine.find("4") != std::string::npos || firstfieldLine.find("5") != std::string::npos ||
                   firstfieldLine.find("6") != std::string::npos) {
                        gameOver = true;
                        currentPiece.isAlive = false;
                        addScore(scoreBoard, playerScore);
                    }
                // Get next piece and create new one
                if(!gameOver) {
                    // Make next piece current
                    currentPiece = nextPiece;
                    attachTetrominoeToField(&currentPiece, &pField);
                    currentPiece.isAlive = true;
                    // Create new next piece
                    std::srand(std::time({}));
                    unsigned int rndPieceNum = std::rand() % (TT_Count);
                    nextPiece = createTetrominoe((TetrominoeType)rndPieceNum);
                    placeTetrominoe(&nextPiece, pField.x, pField.y - (BLOCK_HEIGHT * 4));
                }
            }
        }
        
        // ************************
        // RENDER
        // ************************
        if(render) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Black
            SDL_RenderClear(renderer);
            
            drawField(renderer, &pField);
            drawTetrominoe(renderer, &currentPiece);
            drawTetrominoe(renderer, &nextPiece);
            
            SDL_SetRenderScale(renderer, 3.0f, 3.0f);
            SDL_SetRenderDrawColor(renderer, 50, 100, 255, SDL_ALPHA_OPAQUE);  // Light Blue
            SDL_RenderDebugText(renderer, 150, 50, scoreText.c_str());
            drawScoreBoard(renderer, &scoreBoard);
            if(gameOver) {
                SDL_SetRenderDrawColor(renderer, 255, 102, 51, SDL_ALPHA_OPAQUE);  // Light Red
                SDL_RenderDebugText(renderer, 150, 70, "GAME OVER !!!");
            }
            SDL_SetRenderScale(renderer, 1.0f, 1.0f);

            SDL_RenderPresent(renderer);
            render = false;
        }

        // Calcuation for fixed FPS
        unsigned long frameTime = SDL_GetTicks() - frameStart;
        if(frameTime < FRAME_DELAY ) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // CLEANUP RESOURCES
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // EXIT GAME (GG)
    return EXIT_SUCCESS;
}