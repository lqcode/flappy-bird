//
// Created by nikita on 15.02.2025.
//

#include "Vulkan.h"


namespace eng {
    void Vulkan::init() {
        this->vkInstance = createInstance();
        const auto device{pickGraphicsCard()};
        this->vkDevice = createLogicalDevice(device);
        vkGetDeviceQueue(this->vkDevice, device.familyIndices.graphicsFamily.value(), 0, &this->vkGraphicsQueue);
    }

    VkDevice Vulkan::createLogicalDevice(const PhysicalDevice &physicalDevice) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = physicalDevice.familyIndices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        constexpr float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures features{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &features;

        VkDevice device{VK_NULL_HANDLE};
        auto result{vkCreateDevice(physicalDevice.device, &deviceCreateInfo, nullptr, &device)};
        assertResult(result, "failed to create device");

        return device;
    }

    Vulkan::PhysicalDevice Vulkan::pickGraphicsCard() const {
        uint32_t devicesCount{};
        auto result{vkEnumeratePhysicalDevices(this->vkInstance, &devicesCount, nullptr)};
        assertResult(result, "failed to enumerate physical devices");
        if (devicesCount == 0) {
            throw std::runtime_error("no GPUs with Vulkan support");
        }

        std::vector<VkPhysicalDevice> devices{devicesCount};
        result = vkEnumeratePhysicalDevices(this->vkInstance, &devicesCount, devices.data());
        assertResult(result, "failed to enumerate physical devices");

        for (const auto device: devices) {
            const auto familyIndices = findQueueFamilies(device);
            const PhysicalDevice physDevice{device, familyIndices};
            if (!physDevice.isSuitable()) {
                continue;
            }

            return physDevice;
        }

        throw std::runtime_error("no suitable graphics card found");
    }

    Vulkan::QueueFamilyIndices Vulkan::findQueueFamilies(VkPhysicalDevice device) {
        uint32_t queueFamilyCount{};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        QueueFamilyIndices indices{};
        for (int i{}; i < queueFamilies.size(); ++i) {
            if (const auto qFamily = queueFamilies.at(i); qFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete())
                break;
        }

        return indices;
    }


    void Vulkan::assertResult(const VkResult result, std::string_view msg) {
        if (result != VK_SUCCESS) {
            const auto fmt{boost::format("%1%, result: %2%") % msg % result};
            throw std::runtime_error(fmt.str());
        }
    }

    VkInstance Vulkan::createInstance() {
        uint32_t requiredExtensionsCount{};
        auto requiredExtensions{SDL_Vulkan_GetInstanceExtensions(&requiredExtensionsCount)};
        if (requiredExtensions == nullptr) {
            auto fmt{boost::format("failed to get instance extensions: %1%") % SDL_GetError()};
            throw std::runtime_error(fmt.str());
        }

        std::vector<const char *> enabledExtensions{
            requiredExtensions, requiredExtensions + requiredExtensionsCount
        };

        std::vector<const char *> enabledLayers{};
        if (this->createInfo.debugMode) {
            enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = this->createInfo.applicationName.data();
        appInfo.applicationVersion = this->createInfo.applicationVersion;
        appInfo.pEngineName = this->createInfo.engineName.data();
        appInfo.engineVersion = this->createInfo.engineVersion;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = enabledExtensions.size();
        createInfo.ppEnabledExtensionNames = enabledExtensions.data();
        createInfo.enabledLayerCount = enabledLayers.size();
        createInfo.ppEnabledLayerNames = enabledLayers.data();

        VkInstance instance{};
        if (const auto result{vkCreateInstance(&createInfo, nullptr, &instance)}; result != VK_SUCCESS) {
            auto fmt{boost::format("error occurred vkCreateInstance, result: %1%") % result};
            throw std::runtime_error(fmt.str());
        }

        return instance;
    }
}
