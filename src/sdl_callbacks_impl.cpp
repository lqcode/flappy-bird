//
// Created by nikita on 15.02.2025.
//

#define SDL_MAIN_USE_CALLBACKS
#include <iostream>
#include <SDL3/SDL_main.h>

#include <Main.h>
#include <vector>
#include <string>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    std::vector<std::string> args(argc);
    for (int i = 0; i < argc; ++i) {
        args.at(i) = argv[i];
    }

    auto m = new Main(args);
    *appstate = m;

    const auto result = m->init();

    return result;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    const auto m = static_cast<Main *>(appstate);

    const auto result = m->handleEvent(event);

    return result;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const auto m = static_cast<Main *>(appstate);

    const auto result = m->update();

    return result;
}

void SDL_AppQuit(void *appstate, const SDL_AppResult result) {
    const auto m = static_cast<Main *>(appstate);

    delete m;
}
