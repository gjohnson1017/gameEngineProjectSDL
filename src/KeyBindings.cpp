#include <map>

#include "Engine/KeyBindings.hpp"

void KeyBindings::setKeyBinds(std::map<SDL_Scancode, Action> keyBindMap)
{
    keybinds = keyBindMap;
}

