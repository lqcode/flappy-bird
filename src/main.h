#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>
#include <cstring>

#include "game/Game.h"

#define APP_NAME "Flappy Bird"
#define APP_VERSION "1.0"
#define APP_UINT_VERSION VK_MAKE_VERSION(1,0,0)
#define APP_ID "ru.lqcode.flappybird"
#define APP_ENGINE_NAME "Nope"
#define APP_ENGINE_UINT_VERSION VK_MAKE_VERSION(1,0,0)

#define WINDOW_TITLE "Flappy Bird"
#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 1280

#define TICK_PERIOD_MS uint64_t(16)

typedef struct {
    SDL_Window *window;
    Game *game;
    uint64_t lastUpdateTime;
    VkInstance vkInstance;
} AppState;

bool createInstance(VkInstance *vkInstance);

void updateGame(const AppState *appState);

void printError(const char *msg);

#endif //MAIN_H
