#pragma once

#include <map>
#include <SDL2/SDL_keyboard.h>

// Having an ENUM for action seperately will help add key binding feature to the engine
// Devs can now create a map between Raw keyboard inputs and the desired action linked to it
// Action ENUM will later be used in actionToPhy to map an action to a Physics change for an entity

enum Action{
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
};

class KeyBindings{
    public:
        
        void setKeyBinds(std::map<SDL_Scancode, Action> keyBindMap);
        Action getKeyBind(SDL_Scancode code) { return keybinds[code]; }

    private:
        std::map<SDL_Scancode, Action> keybinds;
};