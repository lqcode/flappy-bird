//
// Created by nikita on 15.02.2025.
//

#ifndef VULKAN_H
#define VULKAN_H

#include <optional>
#include <string>

#include <SDL3/SDL_vulkan.h>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <vulkan/vulkan.h>

namespace eng {
    class Vulkan {
    public:
        struct CreateInfo {
            const bool debugMode;
            std::string_view applicationName;
            const uint32_t applicationVersion;
            std::string_view engineName;
            const uint32_t engineVersion;
        };

        explicit Vulkan(const CreateInfo &createInfo): createInfo(createInfo) {
        }

        ~Vulkan() {
            vkDestroyDevice(this->vkDevice, nullptr);
            vkDestroyInstance(vkInstance, nullptr);
        }

        void init();

    private:
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;

            bool isComplete() const {
                return graphicsFamily.has_value();
            }
        };

        struct PhysicalDevice {
            VkPhysicalDevice device{VK_NULL_HANDLE};
            QueueFamilyIndices familyIndices;

            bool isSuitable() const {
                return familyIndices.isComplete();
            }
        };

        const CreateInfo createInfo;

        VkInstance vkInstance{VK_NULL_HANDLE};

        VkDevice vkDevice{VK_NULL_HANDLE};

        VkQueue vkGraphicsQueue{VK_NULL_HANDLE};

        VkInstance createInstance();

        PhysicalDevice pickGraphicsCard() const;

        static VkDevice createLogicalDevice(const PhysicalDevice &physicalDevice);

        static bool isDeviceSuitable(const PhysicalDevice &device);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        static void assertResult(VkResult result, std::string_view msg);
    };
} // eng

#endif //VULKAN_H
