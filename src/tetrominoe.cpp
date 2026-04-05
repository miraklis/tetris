#include <sys/types.h>
#include <SDL3/SDL_stdinc.h>

#include "tetrominoe.h"
#include "field.h"

void updateTetrominoe(Tetrominoe* t)
{
    float bw = BLOCK_WIDTH;
    float bh = BLOCK_HEIGHT;
    float bt = 1.0f;
    SDL_FColor bc = {1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int strParser = 0;
    unsigned int vertCnt = 0;
    while(strParser < t->shapeLayout.length()) {
        if(t->shapeLayout[strParser]=='.') {
#if DRAWFULL
            float bx = t->x + (int)(strParser % 4) * bw;
            float by = t->y + (int)(strParser / 4) * bh;
            // Block
            t->vertices[vertCnt++] = {{bx, by}, {0.0f, 0.0f, 0.0f, 1.0f}};
            t->vertices[vertCnt++] = {{bx + bw, by}, {0.0f, 0.0f, 0.0f, 1.0f}};
            t->vertices[vertCnt++] = {{bx, by + bh}, {0.0f, 0.0f, 0.0f, 1.0f}};
            t->vertices[vertCnt++] = {{bx, by + bh}, {0.0f, 0.0f, 0.0f, 1.0f}};
            t->vertices[vertCnt++] = {{bx + bw, by}, {0.0f, 0.0f, 0.0f, 1.0f}};
            t->vertices[vertCnt++] = {{bx + bw,by + bh}, {0.0f, 0.0f, 0.0f, 1.0f}};
            // Top Border
            float bdx, bdy;
            bdx = bx;
            bdy = by;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy + bt}, bc };
            // Bottom Border
            bdx = bx;
            bdy = by + bh - bt;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy + bt}, bc };            
            // Left Border
            bdx = bx;
            bdy = by;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy + bh}, bc };
            // Right Border
            bdx = bx + bw - bt;
            bdy = by;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy + bh}, bc };
#endif
            strParser++;
            continue;
        } else {
            float bx = t->x + (int)(strParser % 4) * bw;
            float by = t->y + (int)(strParser / 4) * bh;
            // Block
            t->vertices[vertCnt++] = {{bx, by}, t->color};
            t->vertices[vertCnt++] = {{bx + bw, by}, t->color};
            t->vertices[vertCnt++] = {{bx, by + bh}, t->color};
            t->vertices[vertCnt++] = {{bx, by + bh}, t->color};
            t->vertices[vertCnt++] = {{bx + bw, by}, t->color};
            t->vertices[vertCnt++] = {{bx + bw,by + bh}, t->color};
            // Top Border
            float bdx, bdy;
            bdx = bx;
            bdy = by;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy + bt}, bc };
            // Bottom Border
            bdx = bx;
            bdy = by + bh - bt;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bt}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bw, bdy + bt}, bc };            
            // Left Border
            bdx = bx;
            bdy = by;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy + bh}, bc };
            // Right Border
            bdx = bx + bw - bt;
            bdy = by;
            t->vertices[vertCnt++] = {{bdx, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx, bdy + bh}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy}, bc };
            t->vertices[vertCnt++] = {{bdx + bt, bdy + bh}, bc };
            strParser++;
        }
    }
}

Tetrominoe createTetrominoe(TetrominoeType type)
{
    Tetrominoe t{};
    t.type = type;
    t.isAlive = false;
    t.x = 0.0f;
    t.y = 0.0f;
    t.wx = 0;
    t.wy = 0;

    switch(type) {
        case TT_Line:
            t.shapeLayout.append("..X.");
            t.shapeLayout.append("..X.");
            t.shapeLayout.append("..X.");
            t.shapeLayout.append("..X.");
            t.color = {.r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f};
            break;
        case TT_Square:
            t.shapeLayout.append("....");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append("....");
            t.color = {.r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f};
            break;
        case TT_T:
            t.shapeLayout.append("..X.");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append("..X.");
            t.shapeLayout.append("....");
            t.color = {.r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f};
            break;
        case TT_L1:
            t.shapeLayout.append(".X..");
            t.shapeLayout.append(".X..");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append("....");
            t.color = {.r = 1.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f};
            break;
        case TT_L2:
            t.shapeLayout.append("..X.");
            t.shapeLayout.append("..X.");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append("....");
            t.color = {.r = 0.8f, .g = 0.2f, .b = 0.5f, .a = 1.0f};
            break;
        case TT_h1:
            t.shapeLayout.append(".X..");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append("..X.");
            t.shapeLayout.append("....");
            t.color = {.r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f};
            break;
        case TT_h2:
            t.shapeLayout.append("..X.");
            t.shapeLayout.append(".XX.");
            t.shapeLayout.append(".X..");
            t.shapeLayout.append("....");
            t.color = {.r = 0.3f, .g = 0.2f, .b = 0.9f, .a = 1.0f};
            break;
        default:
            t.shapeLayout.append("....");
            t.shapeLayout.append("....");
            t.shapeLayout.append("....");
            t.shapeLayout.append("....");
            t.color = {.r = 0, .g = 0, .b = 0, .a = 0};
            break;        
    }
    updateTetrominoe(&t);
    return t;
}

void moveTetrominoe(int x, int y, Tetrominoe* t)
{
    float offsetX = x * BLOCK_WIDTH;
    float offsetY = y * BLOCK_HEIGHT;
    t->wx += x;
    t->wy += y;
    t->x += offsetX;
    t->y += offsetY;
    size_t vertSize = SDL_arraysize(t->vertices);
    for(int i=0; i<vertSize; i++) {
        t->vertices[i].position.x += offsetX;
        t->vertices[i].position.y += offsetY;
    }
}

void placeTetrominoe(Tetrominoe* t, float x, float y)
{
    t->wx = 0;
    t->wy = 0;
    float offsetX = x - t->x;
    float offsetY = y - t->y;
    t->x = x;
    t->y = y;
    size_t vertSize = SDL_arraysize(t->vertices);
    for(int i=0; i<vertSize; i++) {
        t->vertices[i].position.x += offsetX;
        t->vertices[i].position.y += offsetY;
    }
}

void rotateTetrominoe(Tetrominoe* t)
{
    rotateLayout(&t->shapeLayout);
    updateTetrominoe(t);
}

void drawTetrominoe(SDL_Renderer* renderer, Tetrominoe* t)
{
    size_t vertSize = SDL_arraysize(t->vertices);
    SDL_RenderGeometry(renderer, NULL, t->vertices, vertSize, NULL, 0);
}

void attachTetrominoeToField(Tetrominoe* t, PlayField* f, int x, int y)
{
    if(x <= 0 || x > FIELD_WIDTH - 4)
        x = (int)(FIELD_WIDTH / 2) - 2;
    if(y < 0 || y > FIELD_HEIGHT - 4)
        y = 0;
    if(x < 0 || y < 0) {
        x = 0;
        y = 0;
    }
    placeTetrominoe(t, 0.0f, 0.0f);
    float offsetX = f->x;
    float offsetY = f->y;
    t->wx = x;
    t->wy = y;
    t->x = (t->wx * BLOCK_WIDTH) + offsetX;
    t->y = (t->wy * BLOCK_HEIGHT) + offsetY;
    size_t vertSize = SDL_arraysize(t->vertices);
    for(int i=0; i<vertSize; i++) {
        t->vertices[i].position.x += t->x;
        t->vertices[i].position.y += t->y;
    }
}

void rotateLayout(std::string* layout) {
    std::string s = *layout;

    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            (*layout)[(i * 4) + j] = s[12 + i - (j * 4)]; // rotate 90 degrees CW
}

// Check for collissions between current shape and field
// The field includes and the stacked pieces
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