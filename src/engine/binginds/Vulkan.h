//
// Created by nikita on 15.02.2025.
//

#ifndef VULKAN_H
#define VULKAN_H

#include <vulkan/vulkan.h>

class Vulkan {
public:
    explicit Vulkan(VkInstance vkInstance) : vkInstance(vkInstance) {
    }

    ~Vulkan() {
        vkDestroyInstance(vkInstance, nullptr);
    }

    void init();

private:
    VkInstance vkInstance;
    VkPhysicalDevice card;

    void pickGraphicsCard();
};


#endif //VULKAN_H
