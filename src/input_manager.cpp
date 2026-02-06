#include "input_manager.hpp"

void InputManager::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit_requested = true;
        }
        if (event.type == SDL_KEYDOWN) {
            auto it = key_bindings.find(event.key.keysym.sym);
            if (it != key_bindings.end()) {
                it->second();
            }
        }
    }
}

void InputManager::bindKey(SDL_Keycode key, ActionCallback callback) {
    key_bindings[key] = callback;
}

bool InputManager::shouldQuit() { 
  return quit_requested; 
}
  
void InputManager::reset() { 
  quit_requested = false; 
}