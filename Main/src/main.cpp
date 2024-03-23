#include <Engine.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <limits> 
#include <cstdint>

// Region Vulkan

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> computeFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && computeFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initVulkan() {
        createInstance();

        setupDebugMessenger();
        std::cerr << "Created debug messenger" << std::endl;
        createSurface();
        std::cerr << "Created surface" << std::endl;
        pickPhysicalDevice();
        std::cerr << "Created physical device" << std::endl;
        createLogicalDevice();
        std::cerr << "Created logical device" << std::endl;
        createSwapChain();
        std::cerr << "Created swapchain" << std::endl;
        createImageViews();
        std::cerr << "Created Image Views" << std::endl;
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);

        vkDestroyDevice(device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }


        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        
        glfwTerminate();
    }


    void initWindow() {
        glfwInit();
        glfwSetErrorCallback(glfwError);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

        if (!window) {
            glfwTerminate();
            throw std::runtime_error("failed to create window!");
        }
    }
private:
    
    static void glfwError(int id, const char* description)
    {
        std::cout << "GLFW ERROR (" << id << "): " << description << std::endl;
    }
private: // Vulkan Initialiazation


    // Instance
    void createInstance() {
        if constexpr (enableValidationLayers) {
            if (!checkValidationLayerSupport()) {
                throw std::runtime_error("validation layers requested, but not available!");
            }
        } 
        
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;       
        
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};

        std::vector<const char*> requiredExtensions = getRequiredExtensions();
        checkRequiredInstanceExtensions(requiredExtensions);

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessenger(debugMessengerInfo);

            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessengerInfo;
        } 
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    // Debug Messenger
    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = 
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;


        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessenger(createInfo);
 
        VkResult result = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug !");
        }
    
    }

    // Surface

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    // Queue Families

    std::pair<std::vector<VkQueueFamilyProperties>, QueueFamilyIndices> findQueueFamilies(VkPhysicalDevice _physicalDevice) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        std::optional<int> graphicsFamily = std::nullopt;
        std::optional<int> computeFamily = std::nullopt;
        std::optional<int> graphicsAndComputeFamily = std::nullopt;
        std::optional<int> graphicsPresentAndComputeFamily = std::nullopt;
        std::optional<int> presentFamily = std::nullopt;
        std::optional<int> graphicsAndPresentFamily = std::nullopt;
        std::optional<int> computeOnlyQueue = std::nullopt;
        std::optional<int> graphicsOnlyQueue = std::nullopt;

        for (const auto& queueFamily : queueFamilies) {
            // if we're going to create a queue with both graphics and compute capabilities, 
            // we need to make sure that the queue can have multiple queuecount 
            if (queueFamily.queueCount > 1 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                graphicsAndComputeFamily = i;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                computeOnlyQueue = i;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                graphicsOnlyQueue = i;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsFamily = i;
            }

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                computeFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, surface, &presentSupport);

            if (queueFamily.queueCount > 0 && presentSupport && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsAndPresentFamily = i;
            }

            // if we're going to create a queue with both graphics and compute capabilities, 
            // we need to make sure that the queue can have multiple queuecount 
            if (queueFamily.queueCount > 1 && presentSupport && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                graphicsPresentAndComputeFamily = i;
            }

            if (presentSupport) {
                indices.presentFamily = i;
            }

        
            i++;
        }

        if (computeOnlyQueue.has_value()) {
            indices.computeFamily = computeOnlyQueue;
        } 
        else if (graphicsPresentAndComputeFamily.has_value()) {
            indices.computeFamily = graphicsPresentAndComputeFamily;
        }
        else if (graphicsAndComputeFamily.has_value()) {
            indices.computeFamily = graphicsAndComputeFamily;
        }
        else {
            indices.computeFamily = computeFamily;
        }
        
        if (indices.computeFamily.has_value() && indices.computeFamily == graphicsPresentAndComputeFamily) {
            indices.graphicsFamily = graphicsPresentAndComputeFamily;
        }
        else if(graphicsAndPresentFamily.has_value()) {
            indices.graphicsFamily = graphicsAndPresentFamily;
        }
        else if (graphicsOnlyQueue.has_value()) {
            indices.graphicsFamily = graphicsOnlyQueue;
        } 
        else if (graphicsAndComputeFamily.has_value()) {
            indices.graphicsFamily = graphicsAndComputeFamily;
        } 
        else {
            indices.graphicsFamily = graphicsFamily;
        }

        if(indices.graphicsFamily.has_value() && indices.graphicsFamily == graphicsPresentAndComputeFamily) {
            indices.presentFamily = graphicsPresentAndComputeFamily;
        }
        else if (graphicsAndPresentFamily.has_value()) {
            indices.presentFamily = graphicsAndPresentFamily;
        }
        else {
            indices.presentFamily = presentFamily;
        }

        return std::pair(queueFamilies, indices);
    }

    // Physical Device

    bool isDeviceSuitable(VkPhysicalDevice _physicalDevice) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

        auto pair = findQueueFamilies(_physicalDevice);

        (void) deviceProperties;
        (void) deviceFeatures;

        auto computeQueueFamily = pair.second.computeFamily;
        auto graphicsQueueFamily = pair.second.graphicsFamily;

        if(computeQueueFamily.has_value() && graphicsQueueFamily.has_value() && computeQueueFamily.value() == graphicsQueueFamily.value()) {
            // check that 2 queues can be created with the same family
            return pair.first[computeQueueFamily.value()].queueCount > 1;
        }

        bool swapChainAdequate = false;
        if (checkDeviceExtensions(_physicalDevice)) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return pair.second.isComplete() && swapChainAdequate;
    }

    int rateDeviceSuitability(VkPhysicalDevice _physicalDevice) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        auto indices = findQueueFamilies(_physicalDevice);

        // Always better to have a separate compute queue
        if (indices.second.computeFamily != indices.second.graphicsFamily) {
            score += 200;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        return score;
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::vector<VkPhysicalDevice> suitableDevices;

        for(const auto& _physicalDevice : devices) {
            if (isDeviceSuitable(_physicalDevice)) {
                suitableDevices.push_back(_physicalDevice);
            }
        }

        if (devices.empty()) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& suitableDevice : suitableDevices) {
            int score = rateDeviceSuitability(suitableDevice);
            candidates.insert(std::make_pair(score, suitableDevice));
        }

        // The candidates were all suitable, so just pick the best one
        physicalDevice = candidates.rbegin()->second;
    }

    // Logical Device

    void createLogicalDevice() {
        auto pair = findQueueFamilies(physicalDevice);
        auto indices = pair.second;
        (void)pair;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        if (indices.graphicsFamily == indices.computeFamily) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
            queueCreateInfo.queueCount = 2;
            float queuePriority = 1.0f;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        } else {
            VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
            graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            graphicsQueueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
            graphicsQueueCreateInfo.queueCount = 1;
            float graphicsQueuePriority = 1.0f;
            graphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
            queueCreateInfos.push_back(graphicsQueueCreateInfo);

            VkDeviceQueueCreateInfo computeQueueCreateInfo{};
            computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            computeQueueCreateInfo.queueFamilyIndex = indices.computeFamily.value();
            computeQueueCreateInfo.queueCount = 1;
            float computeQueuePriority = 1.0f;
            computeQueueCreateInfo.pQueuePriorities = &computeQueuePriority;
            queueCreateInfos.push_back(computeQueueCreateInfo);
        }

        if (indices.presentFamily != indices.graphicsFamily) {
            VkDeviceQueueCreateInfo presentQueueCreateInfo{};
            presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            presentQueueCreateInfo.queueFamilyIndex = indices.presentFamily.value();
            presentQueueCreateInfo.queueCount = 1;
            float presentQueuePriority = 1.0f;
            presentQueueCreateInfo.pQueuePriorities = &presentQueuePriority;
            queueCreateInfos.push_back(presentQueueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

        if (indices.computeFamily == indices.graphicsFamily) {
            vkGetDeviceQueue(device, indices.computeFamily.value(), 1, &computeQueue);
        } else {
            vkGetDeviceQueue(device, indices.computeFamily.value(), 0, &computeQueue);
        }
    }

    // Swapchain

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }


    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageExtent = extent;

        auto pair = findQueueFamilies(physicalDevice);
        auto indices = pair.second;

        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;


        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
        
    }

    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }


private: // Vulkan Utils

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        for(uint32_t i = 0; i < glfwExtensionCount; i++) {
            extensions.emplace_back(glfwExtensions[i]);
        }

        // Required for Mac OS compatibility
        #ifdef _GLFW_COCOA
            extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        #endif
 

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        (void)pUserData;

        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                std::cerr << "[VERBOSE] ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                std::cerr << "[INFO] ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                std::cerr << "[WARNING] ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                std::cerr << "[ERROR] ";
                break;
            default:
                std::cerr << "[UNKNOWN Severity] ";
                break;
        }

        switch (messageType) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                std::cerr << "[GENERAL] ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                std::cerr << "[VALIDATION] ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                std::cerr << "[PERFORMANCE] ";
                break;
            default:
                std::cerr << "[UNKNOWN Type] ";
                break;
        }

        std::cerr << pCallbackData->pMessage << std::endl;

        // if (pCallbackData->objectCount != 0) {
        //     std::cerr << "Objects: " << std::endl;
        //     for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
        //         std::cerr << "\t" << pCallbackData->pObjects[i].objectType << " " << pCallbackData->pObjects[i]. << std::endl;
        //     }
        // }

        return VK_FALSE;
    }

    
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice _physicalDevice) {
        SwapChainSupportDetails details;
        
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, surface, &details.capabilities);
        
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, surface, &presentModeCount, details.presentModes.data());
        }
        
        return details;
    }
    
private: // Vulkan checks

    void checkRequiredInstanceExtensions(const std::vector<const char*>& requiredExtensions) {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "Available extensions:" << std::endl;

        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << std::endl;
        }

        for (const auto& requiredExtension : requiredExtensions) {
            if (std::find_if(extensions.begin(), extensions.end(), [requiredExtension](const VkExtensionProperties& extension) {
                return strcmp(extension.extensionName, requiredExtension) == 0;
            }) == extensions.end()) {
                throw std::runtime_error("Required extension not found: " + std::string(requiredExtension));
            }
        }
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::cout << "Available layers:" << std::endl;

        for (const auto& layer : availableLayers) {
            std::cout << '\t' << layer.layerName << std::endl;
        }

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    bool checkDeviceExtensions(VkPhysicalDevice _physicalDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue computeQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
};

// EndRegion Vulkan

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}