#include <cstring>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_scancode.h>
#include "menu.h"
#include "text.h"

//static const float scale = 3.0f;

void updateMenu(Menu& m)
{
    // const float rowHeight = 10.0f * scale;
    // const float menuHeight = m.items.size() * rowHeight;
    // size_t maxSize = 0;
    // for(auto& i : m.items) {
    //     if(i.length() > maxSize)
    //         maxSize = i.length();
    // }
    // const float menuWidth = maxSize * 8.0f * scale;
    // m.x -= menuWidth / 2.0f;
    // m.y -= menuHeight / 2.0f;
    const float rowHeight = 10.0f * m.scale;
    const float mx = m.x;
    const float my = m.y;
    const float menuWidth = m.w;
    const float menuHeight = m.h;

    // Menu Background
    SDL_FColor menuColor = {0.0f, 0.0f, 1.0f, 1.0f};
    m.vertices[0].position = { mx, my };
    m.vertices[1].position = { mx, my + menuHeight };
    m.vertices[2].position = { mx + menuWidth, my + menuHeight};
    m.vertices[3].position = { mx + menuWidth, my + menuHeight};
    m.vertices[4].position = { mx + menuWidth, my};
    m.vertices[5].position = { mx, my };
    for(int i = 0; i < 6; i++)
        m.vertices[i].color = menuColor;

    // Menu Selected Item
    float selectedItemY = my + m.selectedIndex * rowHeight;
    SDL_FColor selectionColor = {1.0f, 0.0f, 0.5f, 1.0f};
    m.vertices[6].position = { mx, selectedItemY };
    m.vertices[7].position = { mx, selectedItemY + rowHeight };
    m.vertices[8].position = { mx + menuWidth, selectedItemY + rowHeight};
    m.vertices[9].position = { mx + menuWidth, selectedItemY + rowHeight};
    m.vertices[10].position = { mx + menuWidth, selectedItemY};
    m.vertices[11].position = { mx, selectedItemY };
    for(int i = 6; i < 12; i++)
        m.vertices[i].color = selectionColor;
}

void selectItem(Menu& menu, int itemIndex)
{
    if(itemIndex < 0 || itemIndex >= menu.items.size())
        return;
    menu.selectedIndex = itemIndex;
    updateMenu(menu);
}

Menu createMenu(std::vector<std::string>& items, int x, int y, float scale, int* actions, void(*callback)(int, int*))
{
    Menu m;
    // m.x = x;
    // m.y = y;
    m.actions = actions;
    m.callback = callback;
    m.selectedIndex = 0;
    for(auto i : items) {
        m.items.push_back(i);
    }
    m.scale = scale;
    m.h = m.items.size() * 10.0f * m.scale;
    //const float menuHeight = m.items.size() * rowHeight;
    size_t maxSize = 0;
    for(auto& i : m.items) {
        if(i.length() > maxSize)
            maxSize = i.length();
    }
    m.w = maxSize * 8.0f * m.scale;
    m.x = x - (m.w / 2.0f);
    m.y = y - (m.h / 2.0f);
    updateMenu(m);
    return m;
}

void handleMenuInput(SDL_Event& ev, Menu& menu)
{
    static const bool* menuKeyStates = SDL_GetKeyboardState(nullptr);
    static bool previousMenuKeyStates[SDL_SCANCODE_COUNT] = {};

    // Preserve previous key states
    std::memcpy(previousMenuKeyStates, menuKeyStates, SDL_SCANCODE_COUNT);

    SDL_PollEvent(&ev);
    if(menuKeyStates[SDL_SCANCODE_ESCAPE] && !previousMenuKeyStates[SDL_SCANCODE_ESCAPE]) {
        menu.callback(0, menu.actions);
    }
    if(menuKeyStates[SDL_SCANCODE_DOWN] && !previousMenuKeyStates[SDL_SCANCODE_DOWN]) {
        selectItem(menu, menu.selectedIndex+1);
    }
    if(menuKeyStates[SDL_SCANCODE_UP] && !previousMenuKeyStates[SDL_SCANCODE_UP]) {
        selectItem(menu, menu.selectedIndex-1);
    }
    if(menuKeyStates[SDL_SCANCODE_RETURN] && !previousMenuKeyStates[SDL_SCANCODE_RETURN]) {
        menu.callback(menu.selectedIndex + 1, menu.actions);
    }
}

void drawMenu(SDL_Renderer* renderer, Menu* menu)
{
    if(!menu)
        return;
    size_t vertSize = SDL_arraysize(menu->vertices);
    SDL_RenderGeometry(renderer, NULL, menu->vertices, vertSize, NULL, 0);

    for(int i = 0; i < menu->items.size(); i++) {
        drawText(renderer, 
            menu->items[i], 
            {1.0f, 1.0f, 1.0f, 1.0f}, 
            menu->x, (menu->y + (i * 10.0f * menu->scale) + (menu->scale * 2.0f)), menu->scale);
    }
}