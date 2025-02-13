//
// Created by nikita on 12.02.2025.
//

#ifndef GAME_H
#define GAME_H

#include <vulkan/vulkan.h>

class Game {
public:
    explicit Game(VkInstance vkInstance);

    void init();

    void update(unsigned long delta);

    void render();

    void quit();

private:
    VkInstance vkInstance;
};

#endif //GAME_H
