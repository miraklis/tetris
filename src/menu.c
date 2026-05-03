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
    float selectedItemY = menu->y + menu->selectedIndex * menu->fontSize * 2.0f;
    translateMatrix(menu->x, selectedItemY, menu->barModel);
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
    
    const float mx = 0.0f;
    const float my = 0.0f;
    const float menuWidth = m->w;
    const float menuHeight = m->h;

    // Menu Background
    m->backgroundVertices[0] = (Vertex){mx, my, 0.0f, 0.0f, 1.0f, 1.0f};
    m->backgroundVertices[1] = (Vertex){mx, my + menuHeight, 0.0f, 0.0f, 1.0f, 1.0f};
    m->backgroundVertices[2] = (Vertex){mx + menuWidth, my + menuHeight, 1.0f, 0.0f, 0.0f, 1.0f};
    m->backgroundVertices[3] = (Vertex){mx + menuWidth, my + menuHeight, 1.0f, 0.0f, 0.0f, 1.0f};
    m->backgroundVertices[4] = (Vertex){mx + menuWidth, my, 1.0f, 0.0f, 0.0f, 1.0f};
    m->backgroundVertices[5] = (Vertex){mx, my, 0.0f, 0.0f, 1.0f, 1.0f};

    // Menu Selected Item
    float selectedItemY = my;
    m->barVertices[0] = (Vertex){mx, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    m->barVertices[1] = (Vertex){mx, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    m->barVertices[2] = (Vertex){mx + menuWidth, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    m->barVertices[3] = (Vertex){mx + menuWidth, selectedItemY + rowHeight, 0.3f, 0.3f, 0.3f, 1.0f};
    m->barVertices[4] = (Vertex){ mx + menuWidth, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};
    m->barVertices[5] = (Vertex){ mx, selectedItemY, 0.3f, 0.3f, 0.3f, 1.0f};

    GLuint vbo;

    // Background VAO and VBO
    glGenVertexArrays(1, &m->backVao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(m->backVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6, m->backgroundVertices, GL_DYNAMIC_DRAW);

    // Bar VAO and VBO
    glGenVertexArrays(1, &m->barVao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(m->barVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6, m->barVertices, GL_DYNAMIC_DRAW);

    // Projection and Model matrices
    orthoMatrix(0, dm->w, dm->h, 0, -1, 1, m->proj);
    translateMatrix(m->x, m->y, m->menuModel);
    translateMatrix(m->x, m->y, m->barModel);

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

void drawMenu(Menu* menu, GameShader* gameShader, ColoredTextureShader* uiShader)
{
    if(menu == NULL || !menu->visible)
        return;

    useProgram(gameShader->program);
    glUniformMatrix4fv(gameShader->locProj, 1, GL_FALSE, menu->proj);

    // Draw background
    glUniformMatrix4fv(gameShader->locModel, 1, GL_FALSE, menu->menuModel);
    glBindVertexArray(menu->backVao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES);
    // Draw selected item bar
    glUniformMatrix4fv(gameShader->locModel, 1, GL_FALSE, menu->barModel);
    glBindVertexArray(menu->barVao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES);

    for(int i = 0; i < menu->itemsCnt; i++) {
        drawText(menu->items[i], uiShader);
    }

}