//
// Created by nikita on 15.02.2025.
//

#include "Main.h"

SDL_AppResult Main::init() {
   auto args = parseArgs(this->rawArgs);

   auto success = SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_ID);
   if (!success) {
      BOOST_LOG_TRIVIAL(error) << "Failed to set app metadata: " << SDL_GetError();
      return SDL_APP_FAILURE;
   }

   success = SDL_Init(SDL_INIT_VIDEO);
   if (!success) {
      BOOST_LOG_TRIVIAL(error) << "Failed to init SDL subsystems: " << SDL_GetError();
      return SDL_APP_FAILURE;
   }

   const auto window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
   if (window == nullptr) {
      BOOST_LOG_TRIVIAL(error) << "Failed to create SDL3 window: " << SDL_GetError();
      return SDL_APP_FAILURE;
   }
   this->window = new eng::Window(window);

   try {
      const auto instance = createVulkan(args.debugMode);
      this->vulkan = new Vulkan(instance);
      this->vulkan->init();
   } catch (std::runtime_error e) {
      BOOST_LOG_TRIVIAL(error) << "Failed to create vulkan: " << e.what();
      return SDL_APP_FAILURE;
   }

   this->game = new game::Game();
   this->game->init();

   return SDL_APP_CONTINUE;
}

SDL_AppResult Main::handleEvent(const SDL_Event *event) {
   if (event->type == SDL_EVENT_QUIT) {
      return SDL_APP_SUCCESS;
   }

   return SDL_APP_CONTINUE;
}

SDL_AppResult Main::update() {
   const uint64_t currentTime = SDL_GetTicks();

   uint64_t delta = currentTime - this->lastUpdateTime;
   if (const uint64_t updateCyclesCount = delta / UPDATE_PERIOD_MS; updateCyclesCount > 1) {
      BOOST_LOG_TRIVIAL(warning) <<
         "Time elapsed from last update: " << delta << ". It is >=" << updateCyclesCount << " update cycles!";
   }

   bool isContinue = false;
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
   uint32_t instanceExtensionsCount = 0;
   auto instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount);
   if (instanceExtensions == nullptr) {
      auto fmt = boost::format("failed to get instance extensions: %1%") % SDL_GetError();
      throw std::runtime_error(fmt.str());
   }

   BOOST_LOG_TRIVIAL(debug) << "Supported Vulkan instance extensions: ";
   for (int i = 0; i < instanceExtensionsCount; ++i) {
      BOOST_LOG_TRIVIAL(debug) << i + 1 << ". " << instanceExtensions[i];
   }

   std::vector<const char *> enabledLayers = {};
   if (debug) {
      enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
   }

   VkApplicationInfo appInfo{};
   appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   appInfo.pNext = nullptr;
   appInfo.pApplicationName = APP_NAME;
   appInfo.applicationVersion = APP_VERSION_UINT;
   appInfo.pEngineName = ENGINE_NAME;
   appInfo.engineVersion = ENGINE_VERSION;
   appInfo.apiVersion = VK_API_VERSION_1_3;

   VkInstanceCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   createInfo.pNext = nullptr;
   createInfo.pApplicationInfo = &appInfo;
   createInfo.enabledExtensionCount = instanceExtensionsCount;
   createInfo.ppEnabledExtensionNames = instanceExtensions;
   createInfo.enabledLayerCount = enabledLayers.size();
   createInfo.ppEnabledLayerNames = enabledLayers.data();
   createInfo.flags = 0;

   VkInstance instance;
   auto result = vkCreateInstance(&createInfo, nullptr, &instance);
   if (result != VK_SUCCESS) {
      auto fmt = boost::format("error occurred vkCreateInstance, result: %1%") % result;
      throw std::runtime_error(fmt.str());
   }

   return instance;
}

Main::CmdArgs Main::parseArgs(const std::vector<std::string> &rawArgs) {
   CmdArgs args{};
   for (auto it = rawArgs.begin(); it != rawArgs.end(); ++it) {
      const auto &arg = *it;
      if (arg.rfind(ARG_FLAG_SIGN) != 0) {
         continue;
      }

      const std::string flag(arg.begin() + ARG_FLAG_SIGN_LEN, arg.end());
      if (flag == ARG_DEBUG_FLAG) {
         args.debugMode = true;
      } else {
         BOOST_LOG_TRIVIAL(warning) << "Unknown flag " << flag;
      }
   }

   return args;
}
