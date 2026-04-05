#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iterator>
#include <string>
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

bool checkCollissions(Tetrominoe* t, PlayField* f, int offsetX, int offsetY, bool rotated)
{
    int fx = t->wx + offsetX;
    int sx = t->wx + offsetX;
    int y = t->wy + offsetY;

    // Prepare the field mask
    std::string fieldMask = "";
    if(fx < 0) {
        fx = 0;
    }
    if(fx > FIELD_WIDTH - 4) {
        fx = FIELD_WIDTH - 4;
    }

    int cnt = 0;
    while(y < FIELD_HEIGHT && cnt < 4) {
        fieldMask.append(f->fieldLayout.substr((y * FIELD_WIDTH) + fx, 4));
        cnt++;
        y++;
    }

    // Prepare the shape mask    
    std::string shapeMask = t->shapeLayout;
    if(rotated)
        rotateLayout(&shapeMask);

    // In case shape's mask is outside the left/right field limits
    // update the columns of the masks to be checked
    int sc = 0;
    int ec = 4;
    int foffset = 0;
    if(sx < 0) {
        sc = -sx;
        ec = 4;
        foffset = sx;
    }
    if(sx > FIELD_WIDTH -4) {
        sc = 0;
        ec = FIELD_WIDTH - sx;
        foffset = 4 - (FIELD_WIDTH - sx);
    }
    // Iterate the two masks to check for collission
    for(int r=0; r < 4; r++) {
        for(int c=sc; c < ec; c++) {
            if(shapeMask[(r * 4) + c] == 'X' && fieldMask[(r * 4) + c + foffset] != '.') {
                if(rotated) {
                    // Make the piece move by 1 and check again
                    Tetrominoe rt = *t;
                    rotateLayout(&rt.shapeLayout);
                    int step = c >= 2 ? -1 : 1;
                    if(!checkCollissions(&rt, f, step, 0, false)) {
                        moveTetrominoe(step, 0, t);
                        return false;
                    } else {
                        return true;
                    }
                } else {
                    return true; // There is a collission
                }
            }
        }
    }
    return false; // No collissions
}

void removeFilledLines(PlayField* f) {
    int bottom = FIELD_HEIGHT - 1;
    int lineSize = FIELD_WIDTH - 2;
    std::string emptyLine(lineSize, '.');
    std::string filledLine(lineSize, '=');
    while(bottom > 0) {
        bottom--;
        std::string line = f->fieldLayout.substr((bottom * FIELD_WIDTH) + 1, lineSize);
        if(line == filledLine) {
            int yy = bottom;
            while(yy > 1) {
                yy--;
                std::string prevLine = f->fieldLayout.substr((yy * FIELD_WIDTH) + 1, lineSize);
                f->fieldLayout.replace(((yy + 1) * FIELD_WIDTH) + 1, lineSize, prevLine);
            }
            f->fieldLayout.replace(1, lineSize, emptyLine);
            updateField(f);
            bottom++;
        }
    }
}

int checkForFilledLines(PlayField* f, Tetrominoe* t)
{
    int result = 0;
    int top = t->wy;
    int bottom = (t->wy + 4) > FIELD_HEIGHT - 1? FIELD_HEIGHT - 1 : (t->wy + 4);
    int lineSize = FIELD_WIDTH - 2;
    std::string emptyLine(lineSize, '.');
    std::string filledLine(lineSize, '=');
    while(bottom > 0 && bottom >= top) {
        bottom--;
        std::string line = f->fieldLayout.substr((bottom * FIELD_WIDTH) + 1, lineSize);
        if(line.find(".") == std::string::npos) {
            result++;
            f->fieldLayout.replace((bottom * FIELD_WIDTH) + 1, lineSize, filledLine);
        }
    }
    if(result == 4)
      result += 4;
    if(result == 3)
        result += 2;
    if(result == 2)
        result++;
    return result * LINE_FILLED_SCORE;    
}

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
    placeTetrominoe(&next, (f.x + FIELD_WIDTH * BLOCK_WIDTH), BLOCK_WIDTH);

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

    SDL_Event ev = {};
    bool keyPress[4] = {};
    const bool* currentKeyStates = SDL_GetKeyboardState(nullptr);
    bool previousKeyStates[SDL_SCANCODE_COUNT] = {};
    unsigned long repeatDelay[4] = {SDL_GetTicks()};
    while (running)
    {
        unsigned long frameStart = SDL_GetTicks();
        SDL_PollEvent(&ev);
        if(ev.type == SDL_EVENT_QUIT)
            running = false;
        // Escape (exit game)
        if(currentKeyStates[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }
        // F12 (new game)
        if(currentKeyStates[SDL_SCANCODE_F12]) {
            initGame(pField, currentPiece, nextPiece);
        }
        if(currentKeyStates[SDL_SCANCODE_LEFT] && !previousKeyStates[SDL_SCANCODE_LEFT]) {
            keyPress[0] = true;
            repeatDelay[0] = SDL_GetTicks();
        }
        if(currentKeyStates[SDL_SCANCODE_LEFT] && previousKeyStates[SDL_SCANCODE_LEFT]) {
            if(SDL_GetTicks() - repeatDelay[0] >= 100) {
                repeatDelay[0] = SDL_GetTicks();
                keyPress[0] = true;
            }
        }
        if(currentKeyStates[SDL_SCANCODE_RIGHT] && !previousKeyStates[SDL_SCANCODE_RIGHT]) {
            keyPress[1] = true;
            repeatDelay[1] = SDL_GetTicks();
        }
        if(currentKeyStates[SDL_SCANCODE_RIGHT] && previousKeyStates[SDL_SCANCODE_RIGHT]) {
            if(SDL_GetTicks() - repeatDelay[1] >= 100) {
                repeatDelay[1] = SDL_GetTicks();
                keyPress[1] = true;
            }
        }
        if(currentKeyStates[SDL_SCANCODE_DOWN] && !previousKeyStates[SDL_SCANCODE_DOWN]) {
            keyPress[2] = true;
            repeatDelay[2] = SDL_GetTicks();
        }
        if(currentKeyStates[SDL_SCANCODE_DOWN] && previousKeyStates[SDL_SCANCODE_DOWN]) {
            if(SDL_GetTicks() - repeatDelay[2] >= 50) {
                repeatDelay[2] = SDL_GetTicks();
                keyPress[2] = true;
            }
        }
        if(currentKeyStates[SDL_SCANCODE_UP] && !previousKeyStates[SDL_SCANCODE_UP]) {
            keyPress[3] = true;
            repeatDelay[3] = SDL_GetTicks();
        }
        if(currentKeyStates[SDL_SCANCODE_UP] && previousKeyStates[SDL_SCANCODE_UP]) {
            if(SDL_GetTicks() - repeatDelay[3] >= 200) {
                repeatDelay[3] = SDL_GetTicks();
                keyPress[3] = true;
            }
        }
        if(keyPress[0] && currentPiece.isAlive && !checkCollissions(&currentPiece, &pField, -1, 0, false)) {
            moveTetrominoe(-1, 0, &currentPiece);
            render = true;
        }
        if(keyPress[1] && currentPiece.isAlive && !checkCollissions(&currentPiece, &pField, 1, 0, false)) {
            moveTetrominoe(1, 0, &currentPiece);
            render = true;
        }
        if(keyPress[2] && currentPiece.isAlive) {
            if(!checkCollissions(&currentPiece, &pField, 0, 1, false)) {
                moveTetrominoe(0, 1, &currentPiece);
                render = true;
            } else {
                pieceOnStack = true;
            }
        }
        if(keyPress[3] && currentPiece.isAlive && !checkCollissions(&currentPiece, &pField, 0, 0, true)) {
            rotateTetrominoe(&currentPiece);
            render = true;
        }
        SDL_zero(keyPress);
        std::memcpy(previousKeyStates, currentKeyStates, SDL_SCANCODE_COUNT);

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

            // We are shhowing filled lines animation
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
                    playerScore += scoreFromLines;
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
                    placeTetrominoe(&nextPiece, 400.0f, 20.0f);
                }
            }
        }
        
        // If we must render, render
        if(render) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Black
            SDL_RenderClear(renderer);
            
            drawField(renderer, &pField);
            drawTetrominoe(renderer, &currentPiece);
            drawTetrominoe(renderer, &nextPiece);
            
            SDL_SetRenderScale(renderer, 4.0f, 4.0f);
            SDL_SetRenderDrawColor(renderer, 51, 102, 255, SDL_ALPHA_OPAQUE);  // Light Blue
            SDL_RenderDebugText(renderer, 100, 50, scoreText.c_str());
            if(gameOver) {
                SDL_SetRenderDrawColor(renderer, 255, 102, 51, SDL_ALPHA_OPAQUE);  // Light Red
                SDL_RenderDebugText(renderer, 100, 70, "GAME OVER !!!");
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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}