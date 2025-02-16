//
// Created by nikita on 15.02.2025.
//

#ifndef VULKAN_H
#define VULKAN_H

#include <vulkan/vulkan.h>

namespace eng {
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
} // eng

#endif //VULKAN_H
