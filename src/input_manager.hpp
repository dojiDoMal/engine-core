#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <SDL2/SDL.h>
#include <functional>
#include <unordered_map>

class InputManager {
public:
    using ActionCallback = std::function<void()>;

    void processEvents();
    void bindKey(SDL_Keycode key, ActionCallback callback);
    bool shouldQuit();
    void reset();

private:
    bool quit_requested = false;
    std::unordered_map<SDL_Keycode, ActionCallback> key_bindings;
};

#endif