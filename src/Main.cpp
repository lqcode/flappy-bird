//
// Created by nikita on 15.02.2025.
//

#include "Main.h"

SDL_AppResult Main::init() {
   auto success{SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_ID)};
   if (!success) {
      BOOST_LOG_TRIVIAL(error) << "Failed to set app metadata: " << SDL_GetError();
      return SDL_APP_FAILURE;
   }

   success = SDL_Init(SDL_INIT_VIDEO);
   if (!success) {
      BOOST_LOG_TRIVIAL(fatal) << "Failed to init SDL subsystems: " << SDL_GetError();
      return SDL_APP_FAILURE;
   }

   const auto window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
   if (window == nullptr) {
      BOOST_LOG_TRIVIAL(fatal) << "Failed to create SDL3 window: " << SDL_GetError();
      return SDL_APP_FAILURE;
   }
   this->window = new eng::Window(window);

   try {
      const auto instance = createVulkan(this->cmdArgs.debugMode);
      this->vulkan = new eng::Vulkan(instance);
      this->vulkan->init();
   } catch (std::exception &e) {
      BOOST_LOG_TRIVIAL(fatal) << "Failed to create vulkan: " << e.what();
      return SDL_APP_FAILURE;
   }

   this->game = new game::Game();
   try {
      this->game->init();
   } catch (std::exception &e) {
      BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize the game: " << e.what();
      return SDL_APP_FAILURE;
   }

   return SDL_APP_CONTINUE;
}

SDL_AppResult Main::handleEvent(const SDL_Event *event) {
   if (event->type == SDL_EVENT_QUIT) {
      return SDL_APP_SUCCESS;
   }

   return SDL_APP_CONTINUE;
}

SDL_AppResult Main::update() {
   const uint64_t currentTime{SDL_GetTicks()};

   uint64_t delta{currentTime - this->lastUpdateTime};
   if (const uint64_t updateCyclesCount{delta / UPDATE_PERIOD_MS}; updateCyclesCount > 1) {
      BOOST_LOG_TRIVIAL(warning) <<
         "Time elapsed from last update: " << delta << ". It is >=" << updateCyclesCount << " update cycles!";
   }

   bool isContinue{};
   while (delta >= UPDATE_PERIOD_MS) {
      this->lastUpdateTime += UPDATE_PERIOD_MS;

      isContinue = this->game->update(UPDATE_PERIOD_MS);
      if (!isContinue) {
         return SDL_APP_SUCCESS;
      }

      delta -= UPDATE_PERIOD_MS;
   }
   this->game->render();

   return SDL_APP_CONTINUE;
}

VkInstance Main::createVulkan(bool debug) {
   uint32_t instanceExtensionsCount{};
   auto instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount);
   if (instanceExtensions == nullptr) {
      auto fmt{boost::format("failed to get instance extensions: %1%") % SDL_GetError()};
      throw std::runtime_error(fmt.str());
   }

   BOOST_LOG_TRIVIAL(debug) << "Supported Vulkan instance extensions: ";
   for (int i{}; i < instanceExtensionsCount; ++i) {
      BOOST_LOG_TRIVIAL(debug) << i + 1 << ". " << instanceExtensions[i];
   }

   std::vector<const char *> enabledLayers{};
   if (debug) {
      enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
      BOOST_LOG_TRIVIAL(debug) << "Vulkan validation layer is enabled";
   }

   VkApplicationInfo appInfo{};
   appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   appInfo.pApplicationName = APP_NAME;
   appInfo.applicationVersion = APP_VERSION_UINT;
   appInfo.pEngineName = ENGINE_NAME;
   appInfo.engineVersion = ENGINE_VERSION;
   appInfo.apiVersion = VK_API_VERSION_1_3;

   VkInstanceCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   createInfo.pApplicationInfo = &appInfo;
   createInfo.enabledExtensionCount = instanceExtensionsCount;
   createInfo.ppEnabledExtensionNames = instanceExtensions;
   createInfo.enabledLayerCount = enabledLayers.size();
   createInfo.ppEnabledLayerNames = enabledLayers.data();

   VkInstance instance{};
   const auto result{vkCreateInstance(&createInfo, nullptr, &instance)};
   if (result != VK_SUCCESS) {
      auto fmt{boost::format("error occurred vkCreateInstance, result: %1%") % result};
      throw std::runtime_error(fmt.str());
   }

   return instance;
}

