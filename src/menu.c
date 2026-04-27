//#include <string.h>
#include "std.h"
#include "graphics.h"
#include "shaders.h"
#include "text.h"
#include "menu.h"

void selectItem(Menu* menu, int itemIndex)
{
    if(itemIndex < 0 || itemIndex >= menu->itemsCnt)
        return;
    menu->selectedIndex = itemIndex;

    const float mx = menu->x;
    const float menuWidth = menu->w;
    const float rowHeight = menu->h / menu->itemsCnt;    
    float selectedItemY = menu->y + menu->selectedIndex * menu->fontSize * 2.0f;
    menu->vertices[6]  = (Vertex){mx, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    menu->vertices[7]  = (Vertex){mx, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    menu->vertices[8]  = (Vertex){mx + menuWidth, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    menu->vertices[9]  = (Vertex){mx + menuWidth, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    menu->vertices[10] = (Vertex){ mx + menuWidth, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    menu->vertices[11] = (Vertex){ mx, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    glBindBuffer(GL_ARRAY_BUFFER, menu->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6, sizeof(Vertex) * 6, &menu->vertices[6]);

}

Menu* createMenu(char* items, float x, float y, float fontSize)
{
    Menu* m = (Menu*)malloc(sizeof(Menu));
    m->visible = true;
    m->selectedIndex = 0;
    m->fontSize = fontSize;
    m->items = NULL;
    
    // Parse string to calculate items counter
    // and allocate memory for the items (text)
    // delimiter is "|"
    float rowHeight = fontSize * 2.0f;
    float paddingTextY = rowHeight / 2.0f - fontSize / 2.0f;
    float paddingTextX = fontSize * 2.0f;
    size_t maxSize = 0;
    size_t charCnt = 0;
    size_t itemCnt = 0;
    char item[MAX_TEXT];
    memset(item, 0, MAX_TEXT);
    char* it = items;
    while(*it) {
        if(*it != '|') {
            item[charCnt] = *it;
            it++;
            charCnt++;
            continue;
        }
        if(charCnt > maxSize) {
            maxSize = charCnt;
        }
        memset(item, 0, MAX_TEXT);
        itemCnt++;
        charCnt = 0;
        it++;
    }
    m->items = (Text**)malloc(sizeof(Text*) * itemCnt);
    m->itemsCnt = itemCnt;
    m->w = (maxSize * fontSize) + (paddingTextX * 2.0f);
    m->h = m->itemsCnt * rowHeight;
    m->x = x - (m->w / 2.0f);
    m->y = y - (m->h / 2.0f);

    // Parse string to create items (text)
    // and define menu width (maximum item width)
    // delimiter is "|"
    charCnt = 0;
    itemCnt = 0;
    it = items;
    memset(item, 0, MAX_TEXT);
    while(*it) {
        float alignOffsetX = 0.0f;
        if(*it != '|') {
            item[charCnt] = *it;
            it++;
            charCnt++;
            continue;
        }
        if(charCnt < maxSize) {
            alignOffsetX = ((maxSize - charCnt) * fontSize) / 2.0f;
        }
        //m->items[itemCnt]->verts = NULL;
        m->items[itemCnt] = createStaticText(
            item,
            FONT,
            fontSize,
            m->x + paddingTextX + alignOffsetX,
            m->y + (itemCnt * rowHeight) + paddingTextY
        );
        memset(item, 0, MAX_TEXT);
        itemCnt++;
        charCnt = 0;
        it++;
    }
    
    const float mx = m->x;
    const float my = m->y;
    const float menuWidth = m->w;
    const float menuHeight = m->h;

    // Menu Background
    m->vertices[0] = (Vertex){mx, my, 0.0f, 0.0f, 1.0f, 1.0f};
    m->vertices[1] = (Vertex){mx, my + menuHeight, 0.0f, 0.0f, 1.0f, 1.0f};
    m->vertices[2] = (Vertex){mx + menuWidth, my + menuHeight, 1.0f, 0.0f, 0.0f, 1.0f};
    m->vertices[3] = (Vertex){mx + menuWidth, my + menuHeight, 1.0f, 0.0f, 0.0f, 1.0f};
    m->vertices[4] = (Vertex){mx + menuWidth, my, 1.0f, 0.0f, 0.0f, 1.0f};
    m->vertices[5] = (Vertex){mx, my, 0.0f, 0.0f, 1.0f, 1.0f};

    // Menu Selected Item
    float selectedItemY = my + (m->selectedIndex * rowHeight);
    m->vertices[6]  = (Vertex){mx, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    m->vertices[7]  = (Vertex){mx, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    m->vertices[8]  = (Vertex){mx + menuWidth, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    m->vertices[9]  = (Vertex){mx + menuWidth, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    m->vertices[10] = (Vertex){ mx + menuWidth, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    m->vertices[11] = (Vertex){ mx, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};

    glGenVertexArrays(1, &m->vao);
    glGenBuffers(1, &m->vbo);
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER, MENU_VERTICES_COUNT * sizeof(Vertex), m->vertices, GL_DYNAMIC_DRAW);

    orthoMatrix(0, dm->w, dm->h, 0, -1, 1, m->proj);
    translateMatrix(0.0f, 0.0f, m->model);

    return m;
}

void handleMenuInput(const bool* currentKeyStates, Menu* menu)
{
    static bool previousMenuKeyStates[SDL_SCANCODE_COUNT] = {0};
    static bool menuVisible = false;

    menu->action = -1;
    if(currentKeyStates[SDL_SCANCODE_ESCAPE] && !previousMenuKeyStates[SDL_SCANCODE_ESCAPE] && menuVisible) {
        menu->action = 0;
        menuVisible = false;
        return;
    }
    if(currentKeyStates[SDL_SCANCODE_RETURN] && !previousMenuKeyStates[SDL_SCANCODE_RETURN]) {
        menu->action = menu->selectedIndex + 1;
        menuVisible = false;
        return;
    }
    if(currentKeyStates[SDL_SCANCODE_DOWN] && !previousMenuKeyStates[SDL_SCANCODE_DOWN]) {
        selectItem(menu, menu->selectedIndex+1);
    }
    if(currentKeyStates[SDL_SCANCODE_UP] && !previousMenuKeyStates[SDL_SCANCODE_UP]) {
        selectItem(menu, menu->selectedIndex-1);
    }
    menuVisible = menu->visible;
    memcpy(previousMenuKeyStates, currentKeyStates, SDL_SCANCODE_COUNT);    
}

void drawMenu(Menu* menu, GameShader* gameShader, ColoredTextureShader* uiShader)//GLuint program)
{
    if(menu == NULL || !menu->visible)
        return;

    useProgram(gameShader->program);
    glUniformMatrix4fv(gameShader->locProj, 1, GL_FALSE, menu->proj);
    glUniformMatrix4fv(gameShader->locModel, 1, GL_FALSE, menu->model);
    glBindVertexArray(menu->vao);
    glDrawArrays(GL_TRIANGLES, 0, MENU_VERTICES_COUNT);

    for(int i = 0; i < menu->itemsCnt; i++) {
        drawText(menu->items[i], uiShader);
    }

}