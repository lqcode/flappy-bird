//
// Created by nikita on 15.02.2025.
//

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <Main.h>
#include <vector>
#include <string>

#define ARG_FLAG_SIGN "--"
#define ARG_FLAG_SIGN_LEN 2
#define ARG_DEBUG_FLAG "debug"

void parseArgs(int argc, char **argv, Main::CmdArgs *cmdArgs);

SDL_AppResult SDL_AppInit(void **appstate, const int argc, char **argv) {
    auto cmdArgs{new Main::CmdArgs};
    parseArgs(argc, argv, cmdArgs);

    const auto m = new Main(*cmdArgs);
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
    if (result != SDL_APP_SUCCESS) {
        SDL_Log("An fatal error occurred and the application was closed :(");
    }

    const auto m = static_cast<Main *>(appstate);
    delete m;
}

void parseArgs(const int argc, char **argv, Main::CmdArgs *cmdArgs) {
    for (int i{}; i < argc; ++i) {
        const auto &arg = std::string(argv[i]);
        if (arg.rfind(ARG_FLAG_SIGN) != 0) {
            continue;
        }

        const std::string flag(arg.begin() + ARG_FLAG_SIGN_LEN, arg.end());
        if (flag == ARG_DEBUG_FLAG) {
            cmdArgs->debugMode = true;
        }
    }
}
