//
// Created by nikita on 15.02.2025.
//

#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "engine/binginds/Window.h"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <vulkan/vulkan.h>

#include "engine/binginds/Vulkan.h"
#include "game/Game.h"

#define APP_NAME "Flappy Bird"
#define APP_VERSION "1.0"
#define APP_VERSION_UINT VK_MAKE_VERSION(1,0,0)
#define APP_ID "ru.lqcode.flappybird"

#define ENGINE_NAME "prikoles"
#define ENGINE_VERSION VK_MAKE_VERSION(0,0,1)

#define WINDOW_TITLE "Flappy Bird"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_VULKAN

#define ARG_FLAG_SIGN "--"
#define ARG_FLAG_SIGN_LEN 2
#define ARG_DEBUG_FLAG "debug"

#define UPDATE_PERIOD_MS 16ull

class Main {
    struct CmdArgs {
        bool debugMode = false;
    };

public:
    explicit Main(const std::vector<std::string> &rawArgs): rawArgs(rawArgs) {
    }

    ~Main() {
        delete game;
        delete vulkan;
        delete window;
    }

    SDL_AppResult init();

    SDL_AppResult handleEvent(const SDL_Event *event);

    SDL_AppResult update();

private:
    const std::vector<std::string> &rawArgs;

    eng::Window *window = nullptr;

    Vulkan *vulkan = nullptr;

    uint64_t lastUpdateTime = 0;

    game::Game *game = nullptr;

    static CmdArgs parseArgs(const std::vector<std::string> &rawArgs);

    static VkInstance createVulkan(bool debug);
};


#endif //MAIN_H
