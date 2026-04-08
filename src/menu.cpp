#include <cstring>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_scancode.h>
#include "menu.h"

void updateMenu(Menu& m)
{
    const size_t rowHeight = 30;
    const size_t menuHeight = m.items.size() * rowHeight;
    const size_t menuWidth = 300;
    const float mx = m.x;//1024.0f / 2.0f - menuWidth;
    const float my = m.y;//768.0f / 2.0f - menuHeight;

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

Menu createMenu(std::vector<std::string>& items, int x, int y, void(*callback)(int))
{
    Menu m;
    m.x = x;
    m.y = y;
    m.callback = callback;
    m.selectedIndex = 0;
    for(auto i : items) {
        m.items.push_back(i);
    }
    updateMenu(m);
    return m;
}

void addMenuItem(Menu& menu, std::string item)
{
    menu.items.push_back(item);
    updateMenu(menu);
}

void handleMenuInput(SDL_Event& ev, Menu& menu)
{
    static const bool* menuKeyStates = SDL_GetKeyboardState(nullptr);
    static bool previousMenuKeyStates[SDL_SCANCODE_COUNT] = {};

    // Preserve previous key states
    std::memcpy(previousMenuKeyStates, menuKeyStates, SDL_SCANCODE_COUNT);

    SDL_PollEvent(&ev);
    if(menuKeyStates[SDL_SCANCODE_ESCAPE] && !previousMenuKeyStates[SDL_SCANCODE_ESCAPE]) {
        menu.callback(-1);
    }
    if(menuKeyStates[SDL_SCANCODE_DOWN] && !previousMenuKeyStates[SDL_SCANCODE_DOWN]) {
        selectItem(menu, menu.selectedIndex+1);
    }
    if(menuKeyStates[SDL_SCANCODE_UP] && !previousMenuKeyStates[SDL_SCANCODE_UP]) {
        selectItem(menu, menu.selectedIndex-1);
    }
    if(menuKeyStates[SDL_SCANCODE_RETURN] && !previousMenuKeyStates[SDL_SCANCODE_RETURN]) {
        menu.callback(menu.selectedIndex);
    }
}

void drawMenu(SDL_Renderer* renderer, Menu* menu)
{
    size_t vertSize = SDL_arraysize(menu->vertices);
    SDL_RenderGeometry(renderer, NULL, menu->vertices, vertSize, NULL, 0);

    float paddingX = 15.0f;
    float paddingY = 5.0f;

    int scaleText = 3.0f;
    SDL_SetRenderScale(renderer, scaleText, scaleText);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); // White
    for(int i = 0; i < menu->items.size(); i++) {
        SDL_RenderDebugText(renderer, 
            (menu->x + paddingX) / scaleText,
            (menu->y + (i * 30) + paddingY) / scaleText,
            menu->items[i].c_str()
        );
    }
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

}