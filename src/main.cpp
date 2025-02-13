#include "main.h"

SDL_AppResult SDL_AppInit(void **ppAppState, int argc, char **argv) {
    auto *appState = new AppState{};
    *ppAppState = appState;

    bool success = SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_ID);
    if (!success) {
        printError("Failed to set app metadata");
        return SDL_APP_FAILURE;
    }

    success = SDL_Init(SDL_INIT_VIDEO);
    if (!success) {
        printError("Failed to init SDL");
        return SDL_APP_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == nullptr) {
        printError("Failed to create window");
        return SDL_APP_FAILURE;
    }
    appState->window = window;

    VkInstance vkInstance = nullptr;
    success = createInstance(&vkInstance);
    if (!success) {
        return SDL_APP_FAILURE;
    }
    appState->vkInstance = vkInstance;

    appState->game = new Game(vkInstance);
    appState->game->init();

    appState->lastUpdateTime = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

bool createInstance(VkInstance *vkInstance) {
    uint32_t countInstanceExtensions;
    const char *const *instanceExtension = SDL_Vulkan_GetInstanceExtensions(&countInstanceExtensions);
    if (instanceExtension == nullptr) {
        printError("Failed to GetInstanceExtensions");
        return false;
    }

    constexpr VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = APP_NAME,
        .applicationVersion = APP_UINT_VERSION,
        .pEngineName = APP_ENGINE_NAME,
        .engineVersion = APP_ENGINE_UINT_VERSION,
        .apiVersion = VK_API_VERSION_1_0,
    };

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = countInstanceExtensions;
    createInfo.ppEnabledExtensionNames = instanceExtension;
    createInfo.enabledLayerCount = 0;

    VkInstance instance;
    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        printError("Failed to create vk instance");
        return false;
    }
    *vkInstance = instance;

    return true;
}


SDL_AppResult SDL_AppIterate(void *pAppState) {
    auto *appState = static_cast<AppState *>(pAppState);

    updateGame(appState);
    appState->game->render();

    return SDL_APP_CONTINUE;
}

void updateGame(const AppState *appState) {
    const uint64_t currentTime = SDL_GetTicks();

    uint64_t delta = currentTime - appState->lastUpdateTime;
    while (delta >= TICK_PERIOD_MS) {
        delta -= TICK_PERIOD_MS;

        appState->game->update(TICK_PERIOD_MS);
    }
}


SDL_AppResult SDL_AppEvent(void *pAppState, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *pAppState, const SDL_AppResult result) {
    if (result != SDL_APP_SUCCESS) {
        return;
    }
    const auto *appState = static_cast<AppState *>(pAppState);

    appState->game->quit();
    delete appState->game;

    vkDestroyInstance(appState->vkInstance, nullptr);
    SDL_DestroyWindow(appState->window);
}

void printError(const char *msg) {
    const char *err = SDL_GetError();
    if (std::strlen(err) == 0) {
        SDL_Log("[ERROR] %s", msg);
        return;
    }

    SDL_Log("[SDL ERROR] %s: %s", msg, err);
}

