#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

#include <string>
#include "renderer.hpp"
#include <SDL2/SDL.h>

class WindowManager{
private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    Renderer* renderer = nullptr;
    std::string title = "Custom Engine";
    int width = 800;
    int height = 600;

    bool init(GraphicsAPI);

public:
    WindowManager(GraphicsAPI);
    ~WindowManager();

    Renderer* getRenderer(){return renderer;}
    SDL_Window* getWindow(){return window;}
    void setWindow(SDL_Window* window){this->window = window;}
};

#endif // WINDOWMANAGER_HPP
