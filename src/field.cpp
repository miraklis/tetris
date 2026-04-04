#include "field.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <cstddef>

void updateField(PlayField* f)
{
    f->vertices.clear();
    float bw = BLOCK_WIDTH;
    float bh = BLOCK_HEIGHT;
    unsigned int strParser = 0;
    SDL_FColor vertColor;
    float bt = 2.0f;
    SDL_FColor bc = {1.0f, 1.0f, 1.0f, 1.0f};

    while(strParser < f->fieldLayout.length()) {
        if(f->fieldLayout[strParser]=='.' || f->fieldLayout[strParser]=='@') {
            strParser++;
            continue;
        } else {
            SDL_Vertex v[30];
            if(f->fieldLayout[strParser]=='#') {
                bt = 2.0f;
                vertColor = WALL_COLOR;
            }
            else {
                char c = f->fieldLayout[strParser];
                bt = 1.0f;
                switch(c) {
                    case '0':
                        vertColor = {.r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f};
                        break;
                    case '1':
                        vertColor = {.r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f};
                        break;
                    case '2':
                        vertColor = {.r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f};
                        break;
                    case '3':
                        vertColor = {.r = 1.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f};
                        break;
                    case '4':
                        vertColor = {.r = 0.8f, .g = 0.2f, .b = 0.5f, .a = 1.0f};
                        break;
                    case '5':
                        vertColor = {.r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f};
                        break;
                    case '6':
                        vertColor = {.r = 0.3f, .g = 0.2f, .b = 0.9f, .a = 1.0f};
                        break;
                    default:
                        vertColor = STACK_COLOR;
                        break;
                }
            }
            float bx = f->x + ((int)(strParser % FIELD_WIDTH) * bw);
            float by = f->y + ((int)(strParser / FIELD_WIDTH) * bh);
            // Block
            v[0] = {{bx, by}, vertColor};
            v[1] = {{bx + bw, by}, vertColor};
            v[2] = {{bx, by + bh}, vertColor};
            v[3] = {{bx, by + bh}, vertColor};
            v[4] = {{bx + bw, by}, vertColor};
            v[5] = {{bx + bw,by + bh}, vertColor};
            // Top Border
            float bdx, bdy;
            bdx = bx;
            bdy = by;
            v[6] = {{bdx, bdy}, bc };
            v[7] = {{bdx + bw, bdy}, bc };
            v[8] = {{bdx, bdy + bt}, bc };
            v[9] = {{bdx, bdy + bt}, bc };
            v[10] = {{bdx + bw, bdy}, bc };
            v[11] = {{bdx + bw, bdy + bt}, bc };
            // Bottom Border
            bdx = bx;
            bdy = by + bh - bt;
            v[12] = {{bdx, bdy}, bc };
            v[13] = {{bdx + bw, bdy}, bc };
            v[14] = {{bdx, bdy + bt}, bc };
            v[15] = {{bdx, bdy + bt}, bc };
            v[16] = {{bdx + bw, bdy}, bc };
            v[17] = {{bdx + bw, bdy + bt}, bc };            
            // Left Border
            bdx = bx;
            bdy = by;
            v[18] = {{bdx, bdy}, bc };
            v[19] = {{bdx + bt, bdy}, bc };
            v[20] = {{bdx, bdy + bh}, bc };
            v[21] = {{bdx, bdy + bh}, bc };
            v[22] = {{bdx + bt, bdy}, bc };
            v[23] = {{bdx + bt, bdy + bh}, bc };
            // Right Border
            bdx = bx + bw - bt;
            bdy = by;
            v[24] = {{bdx, bdy}, bc };
            v[25] = {{bdx + bt, bdy}, bc };
            v[26] = {{bdx, bdy + bh}, bc };
            v[27] = {{bdx, bdy + bh}, bc };
            v[28] = {{bdx + bt, bdy}, bc };
            v[29] = {{bdx + bt, bdy + bh}, bc };
            size_t vertArraySize = SDL_arraysize(v);
            for(int i=0; i<vertArraySize; i++)
                f->vertices.push_back(v[i]);
            strParser++;
        }
    }    
}

PlayField createField(float x, float y)
{
    PlayField f;
    for(int i=0; i < FIELD_HEIGHT - 1; i++) {
        char c = i >=START_FIELD_ROW ? '#' : '@';
        f.fieldLayout.append(c+std::string((FIELD_WIDTH - 2), '.')+c);
    }
    f.fieldLayout.append(std::string(FIELD_WIDTH, '#'));
    f.x = x;
    f.y = y;
    updateField(&f);
    return f;
}

void drawField(SDL_Renderer* renderer, PlayField* f)
{
    size_t vertSize = f->vertices.size();
    SDL_RenderGeometry(renderer, NULL, f->vertices.data(), vertSize, NULL, 0);
}
