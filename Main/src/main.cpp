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
#include <fstream>
#include <array>
#include <functional>
#include <sstream>
#include <chrono>
// Region Vulkan

#include "queue_families.hpp"
#include "renderPipeline.hpp"
#include "presentPipeline.hpp"

const int WIDTH = 800;
const int HEIGHT = 800;

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

struct PresentVertex {
    glm::vec2 pos;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(PresentVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(PresentVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(PresentVertex, texCoord);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};


struct Image {
    VkImage image;
    VmaAllocation allocation;
    VkImageView imageView;
    VkSampler sampler;
    VkExtent2D extent;
};

struct RenderTarget : public Image {
    VkFramebuffer framebuffer;
};


const std::vector<RenderPipeline::Vertex> renderTargetVertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint16_t> renderTargetIndices = {
    0, 1, 2, 2, 3, 0};

const std::vector<PresentPipeline::Vertex> presentVertices = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> presentIndices = {
    0, 1, 2, 2, 3, 0
};

class HelloTriangleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initVulkan()
    {
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
        createRenderPass();
        std::cerr << "Created Render Pass" << std::endl;
        createDescriptorSetLayout(); 
        std::cerr << "Created Descriptor Set Layout" << std::endl;
        createRenderPipeline();
        std::cerr << "Created Graphics Pipeline" << std::endl;
        createFramebuffers();
        std::cerr << "Created Framebuffers" << std::endl;
        createCommandPool();
        std::cerr << "Created Command Pool" << std::endl;
        createVMAAllocator();
        std::cerr << "Created VMA Allocator" << std::endl;
        createVertexBuffer();
        std::cerr << "Created Vertex Buffer" << std::endl;
        createIndexBuffer();
        std::cerr << "Created Index Buffer" << std::endl;
        createUniformBuffers();
        std::cerr << "Created Uniform Buffers" << std::endl;
        createRenderTargets();
        std::cerr << "Created Render Targets" << std::endl;
        createDescriptorPool();
        std::cerr << "Created Descriptor Pool" << std::endl;
        createDescriptorSets();
        std::cerr << "Created Descriptor Sets" << std::endl;
        createCommandBuffer();
        std::cerr << "Created Command Buffer" << std::endl;
        createSyncObjects();
        std::cerr << "Created Sync Objects" << std::endl;
    }

    void mainLoop()
    {

        while (!glfwWindowShouldClose(window))
        {

            glfwPollEvents();

            auto currentTime = std::chrono::steady_clock::now();
            duration elapsed = currentTime - previousTime;
            previousTime = currentTime;

            this->frameTime += elapsed.count();

            if (this->frameTime >= 1.0)
            {
                double fps = (double)frameCount;

                std::stringstream ss;
                ss << "Vulkan!"
                   << " [" << fps << " FPS]";

                glfwSetWindowTitle(window, ss.str().c_str());

                frameCount = 0;
                frameTime -= 1.0;
            }

            drawFrame();

            ++frameCount;
            previousTime = currentTime;
        }
        vkDeviceWaitIdle(device);
    }

    

    void cleanup()
    {

        cleanupSwapChain();
        
        vkDestroyDescriptorPool(device, renderDescriptorPool, nullptr);
        vkDestroyDescriptorPool(device, presentDescriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(device, renderDescriptorSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(device, presentDescriptorSetLayout, nullptr);


        for(auto& uniformBuffer : uniformBuffers) {
            vmaUnmapMemory(allocator, uniformBuffer.allocation);
            vmaDestroyBuffer(allocator, uniformBuffer.buffer, uniformBuffer.allocation);
        }

        for(auto& renderTarget : renderTargets) {
            vkDestroyFramebuffer(device, renderTarget.framebuffer, nullptr);
            vkDestroyImageView(device, renderTarget.imageView, nullptr);
            vkDestroySampler(device, renderTarget.sampler, nullptr);
            vmaDestroyImage(allocator, renderTarget.image, renderTarget.allocation);
        }

        vmaDestroyBuffer(allocator, renderVertexBuffer.buffer, renderVertexBuffer.allocation);
        vmaDestroyBuffer(allocator, renderIndexBuffer.buffer, renderIndexBuffer.allocation);

        vmaDestroyBuffer(allocator, presentVertexBuffer.buffer, presentVertexBuffer.allocation);
        vmaDestroyBuffer(allocator, presentIndexBuffer.buffer, presentIndexBuffer.allocation);
        
        vmaDestroyAllocator(allocator);

        for (size_t i = 0; i < framesInFlight; i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, graphicsCommandPool, nullptr);
        vkDestroyCommandPool(device, transferCommandPool, nullptr);
        

        renderPipeline.cleanup();
        presentPipeline.cleanup();

        vkDestroyRenderPass(device, renderPass, nullptr);

        vkDestroyDevice(device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);

        if (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void cleanupRenderTargets() {
        for(auto& renderTarget : renderTargets) {
            vkDestroyFramebuffer(device, renderTarget.framebuffer, nullptr);
            vkDestroyImageView(device, renderTarget.imageView, nullptr);
            vmaDestroyImage(allocator, renderTarget.image, renderTarget.allocation);
        }
    }

    void cleanupSwapChain()
    {
        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);
    }

    void recreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device);

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createFramebuffers();

        resizeRenderTargets();
    }

    void initWindow()
    {
        glfwInit();
        glfwSetErrorCallback(glfwError);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("failed to create window!");
        }

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

        previousTime = std::chrono::steady_clock::now();
    }

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        (void)width;
        (void)height;
        auto app = reinterpret_cast<HelloTriangleApplication *>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

private:
    static void glfwError(int id, const char *description)
    {
        std::cout << "GLFW ERROR (" << id << "): " << description << std::endl;
    }

private: // Vulkan Initialiazation
    // Instance
    void createInstance()
    {
        if constexpr (enableValidationLayers)
        {
            if (!checkValidationLayerSupport())
            {
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

        std::vector<const char *> requiredExtensions = getRequiredExtensions();
        checkRequiredInstanceExtensions(requiredExtensions);

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessenger(debugMessengerInfo);

            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugMessengerInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }

    // Debug Messenger
    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {

        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }

    void setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessenger(createInfo);

        VkResult result = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug !");
        }
    }

    // Surface

    void createSurface()
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    // Queue Families

    // Physical Device

    bool isDeviceSuitable(VkPhysicalDevice _physicalDevice)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

        auto queueIndices = findQueueFamilies(_physicalDevice, surface);

        (void)deviceProperties;
        (void)deviceFeatures;

        bool swapChainAdequate = false;
        if (checkDeviceExtensions(_physicalDevice))
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return queueIndices.isComplete() && swapChainAdequate;
    }

    int rateDeviceSuitability(VkPhysicalDevice _physicalDevice)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(_physicalDevice, &deviceFeatures);

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        auto indices = findQueueFamilies(_physicalDevice, surface);

        // Always better to have a separate compute queue
        if (indices.computeFamily.value().family != indices.graphicsFamily.value().family)
        {
            score += 200;
        }

        // Even better to have a transfer queue
        if (indices.transferFamily.value().family != indices.graphicsFamily.value().family && indices.transferFamily.value().family != indices.computeFamily.value().family)
        {
            score += 100;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        return score;
    }

    void pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::vector<VkPhysicalDevice> suitableDevices;

        for (const auto &_physicalDevice : devices)
        {
            if (isDeviceSuitable(_physicalDevice))
            {
                suitableDevices.push_back(_physicalDevice);
            }
        }

        if (devices.empty())
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto &suitableDevice : suitableDevices)
        {
            int score = rateDeviceSuitability(suitableDevice);
            candidates.insert(std::make_pair(score, suitableDevice));
        }

        // The candidates were all suitable, so just pick the best one
        physicalDevice = candidates.rbegin()->second;
    }

    // Logical Device

    void createLogicalDevice()
    {
        auto indices = findQueueFamilies(physicalDevice, surface);
        queueFamilyIndices = indices;

        std::cerr << "Graphics Family: " << indices.graphicsFamily.value().family << " (index: " << indices.graphicsFamily.value().queueIndex << ")\n"
                  << "Present Family: " << indices.presentFamily.value().family << " (index: " << indices.presentFamily.value().queueIndex << ")\n"
                  << "Compute Family: " << indices.computeFamily.value().family << " (index: " << indices.computeFamily.value().queueIndex << ")\n"
                  << "Transfer Family: " << indices.transferFamily.value().family << " (index: " << indices.transferFamily.value().queueIndex << ")" << std::endl;

        auto queueCreateInfos = indices.createQueueCreateInfos();
        std::vector<std::vector<float>> queuePriorities;

        for (const auto &queueFamily : queueCreateInfos)
        {
            queuePriorities.push_back(std::vector<float>(queueFamily.queueCount, 1.0f));
        }

        for (size_t i = 0; i < queueCreateInfos.size(); i++)
        {
            queueCreateInfos[i].pQueuePriorities = queuePriorities[i].data();
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value().family, indices.graphicsFamily.value().queueIndex, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value().family, indices.presentFamily.value().queueIndex, &presentQueue);
        vkGetDeviceQueue(device, indices.computeFamily.value().family, indices.computeFamily.value().queueIndex, &computeQueue);
        vkGetDeviceQueue(device, indices.transferFamily.value().family, indices.transferFamily.value().queueIndex, &transferQueue);
    }

    // Swapchain

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            (void)availablePresentMode;
            // if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            // {
            //     return availablePresentMode;
            // }
        }

        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        std::cerr << "Chose Surface Format: " << surfaceFormat.format << " | " << surfaceFormat.colorSpace << std::endl;
        std::cerr << "Chose Present Mode: " << presentMode << std::endl;
        std::cerr << "Chose Extent: " << extent.width << "x" << extent.height << std::endl;

        uint32_t imageCount = 3;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.minImageCount + 1;
        }

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
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
        

        auto indices = queueFamilyIndices;
        framesInFlight = imageCount;
        std::cerr << "Frames in Flight: " << framesInFlight << std::endl;

        uint32_t _queueFamilyIndices[] = {indices.graphicsFamily.value().family, indices.presentFamily.value().family};
        if (indices.graphicsFamily.value().family != indices.presentFamily.value().family)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = _queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
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

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    // Render Pass

    void createRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency selfDependency{};
        selfDependency.srcSubpass = 0;
        selfDependency.dstSubpass = 0;
        selfDependency.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        selfDependency.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        selfDependency.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        selfDependency.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
        selfDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


        VkSubpassDependency dependencies[] = {dependency, selfDependency};

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies;
        
        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    // Descriptor Set Layout

    void createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &renderDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &samplerLayoutBinding;

        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &presentDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

    }

    // Graphics Pipeline



    void createRenderPipeline()
    {
        renderPipeline.init({
            device,
            renderPass,
            swapChainExtent,
            {renderDescriptorSetLayout},
        });

        presentPipeline.init({
            device,
            renderPass,
            swapChainExtent,
            {presentDescriptorSetLayout},
        });
    }

    // Framebuffers
    void createFramebuffers()
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = {
                swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    // Command Pool
    void createCommandPool()
    {
        auto indices = queueFamilyIndices;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = indices.graphicsFamily.value().family;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics command pool!");
        }

        poolInfo.queueFamilyIndex = indices.transferFamily.value().family;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // Optional

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &transferCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }

    // Vulkan Memory Allocator

    void createVMAAllocator()
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = physicalDevice;
        allocatorInfo.device = device;
        allocatorInfo.instance = instance;

        vmaCreateAllocator(&allocatorInfo, &allocator);
    }

    // Vertex Buffer

    void createVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(renderTargetVertices[0]) * renderTargetVertices.size();
        CreateBufferInfo bufferCreateInfo = {};
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        bufferCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferCreateInfo.queueFamilyIndices = {
            queueFamilyIndices.graphicsFamily.value().family,
            queueFamilyIndices.transferFamily.value().family};

        StagingBuffer stagingBuffer = {};

        createBuffer(bufferCreateInfo, stagingBuffer.allocation, stagingBuffer.buffer);

        void *data;
        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, renderTargetVertices.data(), (size_t)bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        bufferCreateInfo.flags = {};

        createBuffer(bufferCreateInfo, renderVertexBuffer.allocation, renderVertexBuffer.buffer);

        copyBuffer(stagingBuffer, renderVertexBuffer, bufferSize);
        
        vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);

        bufferSize = sizeof(presentVertices[0]) * presentVertices.size();
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        bufferCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferCreateInfo.queueFamilyIndices = {
            queueFamilyIndices.graphicsFamily.value().family,
            queueFamilyIndices.transferFamily.value().family};

        createBuffer(bufferCreateInfo, stagingBuffer.allocation, stagingBuffer.buffer);

        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, presentVertices.data(), (size_t)bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        bufferCreateInfo.flags = {};

        createBuffer(bufferCreateInfo, presentVertexBuffer.allocation, presentVertexBuffer.buffer);

        copyBuffer(stagingBuffer, presentVertexBuffer, bufferSize);

        vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);
    }

    // Index Buffer

    void createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(renderTargetIndices[0]) * renderTargetIndices.size();
        CreateBufferInfo bufferCreateInfo = {};
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        bufferCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferCreateInfo.queueFamilyIndices = {
            queueFamilyIndices.graphicsFamily.value().family,
            queueFamilyIndices.transferFamily.value().family};

        StagingBuffer stagingBuffer = {};

        createBuffer(bufferCreateInfo, stagingBuffer.allocation, stagingBuffer.buffer);

        void *data;
        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, renderTargetIndices.data(), (size_t)bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        bufferCreateInfo.flags = {};

        createBuffer(bufferCreateInfo, renderIndexBuffer.allocation, renderIndexBuffer.buffer);

        copyBuffer(stagingBuffer, renderIndexBuffer, bufferSize);

        vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);

        bufferSize = sizeof(presentIndices[0]) * presentIndices.size();
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        bufferCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferCreateInfo.queueFamilyIndices = {
            queueFamilyIndices.graphicsFamily.value().family,
            queueFamilyIndices.transferFamily.value().family};

        createBuffer(bufferCreateInfo, stagingBuffer.allocation, stagingBuffer.buffer);

        vmaMapMemory(allocator, stagingBuffer.allocation, &data);
        memcpy(data, presentIndices.data(), (size_t)bufferSize);
        vmaUnmapMemory(allocator, stagingBuffer.allocation);

        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        bufferCreateInfo.flags = {};

        createBuffer(bufferCreateInfo, presentIndexBuffer.allocation, presentIndexBuffer.buffer);

        copyBuffer(stagingBuffer, presentIndexBuffer, bufferSize);

        vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);
    }

    // Uniform Buffer 

    void createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(framesInFlight);

        for (size_t i = 0; i < framesInFlight; i++) {
            CreateBufferInfo bufferCreateInfo = {};
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            bufferCreateInfo.memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            bufferCreateInfo.flags = {};
            bufferCreateInfo.queueFamilyIndices = {};


            createBuffer(bufferCreateInfo, uniformBuffers[i].allocation, uniformBuffers[i].buffer);

            vmaMapMemory(allocator, uniformBuffers[i].allocation, &uniformBuffers[i].mapped);
        }
    }

    // Descriptor Pool

    void createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &renderDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());

        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &presentDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

    }

    // Descriptor Sets

    void createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), renderDescriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = renderDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        renderDescriptorSets.resize(swapChainImages.size());
        if (vkAllocateDescriptorSets(device, &allocInfo, renderDescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = renderDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }

        createPresentDescriptorSets();
    }

    void createPresentDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), presentDescriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());

        allocInfo.descriptorPool = presentDescriptorPool;
        allocInfo.pSetLayouts = layouts.data();

        presentDescriptorSets.resize(swapChainImages.size());
        if (vkAllocateDescriptorSets(device, &allocInfo, presentDescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        updatePresentDescriptorSets();
    }

    void updatePresentDescriptorSets() {
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            // Image 
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = renderTargets[i].imageView;
            imageInfo.sampler = renderTargets[i].sampler;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = presentDescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }
    }

    // Create render targets 
    void createRenderTargets() {
        const VkExtent2D extent = swapChainExtent;

        renderTargets.resize(framesInFlight);

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = extent.width;
        imageInfo.extent.height = extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = swapChainImageFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        for (size_t i = 0; i < framesInFlight; i++) {
            if (vmaCreateImage(allocator, &imageInfo, &allocInfo, &renderTargets[i].image, &renderTargets[i].allocation, nullptr) != VK_SUCCESS) {
                throw std::runtime_error("failed to create render target image!");
            }

            renderTargets[i].extent = extent;

            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = renderTargets[i].image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapChainImageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;


            if (vkCreateImageView(device, &viewInfo, nullptr, &renderTargets[i].imageView) != VK_SUCCESS) {
                throw std::runtime_error("failed to create render target image view!");
            }

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = &renderTargets[i].imageView;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &renderTargets[i].framebuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }

            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.anisotropyEnable = VK_FALSE;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &renderTargets[i].sampler) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture sampler!");
            }


        }
    }

    // Resize render targets

    void resizeRenderTargets() {
        cleanupRenderTargets();
        createRenderTargets();

        updatePresentDescriptorSets();
    }


    void copyBuffer(Buffer &srcBuffer, Buffer &dstBuffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = transferCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;

        vkCmdCopyBuffer(commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(transferQueue);

        vkFreeCommandBuffers(device, transferCommandPool, 1, &commandBuffer);
    }

    void createBuffer(CreateBufferInfo bufferCreateInfo,
                      VmaAllocation &allocation,
                      VkBuffer &buffer)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferCreateInfo.size;
        bufferInfo.usage = bufferCreateInfo.usage;

        std::set<uint32_t> uniqueQueueFamilyIndices(bufferCreateInfo.queueFamilyIndices.begin(), bufferCreateInfo.queueFamilyIndices.end());

        if (uniqueQueueFamilyIndices.size() > 1)
        {
            bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
            bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
            bufferInfo.pQueueFamilyIndices = bufferCreateInfo.queueFamilyIndices.data();
        }
        else
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = bufferCreateInfo.memoryUsage;
        allocInfo.flags = bufferCreateInfo.flags;

        if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }
    }

    // Command Buffers
    void createCommandBuffer()
    {
        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = graphicsCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(_commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = renderTargets[imageIndex].framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = renderTargets[imageIndex].extent;

        VkClearValue renderClearValue = {{{(float)(0x4A) / 255.f, (float)(0x41) / 255.f, (float)(0x2A) / 255.f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &renderClearValue;

        vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPipeline.getPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(renderTargets[imageIndex].extent.width);
        viewport.height = static_cast<float>(renderTargets[imageIndex].extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = renderTargets[imageIndex].extent;
        vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

        VkBuffer renderVertexBuffers[] = {renderVertexBuffer.buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(_commandBuffer, 0, 1, renderVertexBuffers, offsets);

        vkCmdBindIndexBuffer(_commandBuffer, renderIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPipeline.getPipelineLayout(), 0, 1, &renderDescriptorSets[currentFrame], 0, nullptr);

        vkCmdDrawIndexed(_commandBuffer, static_cast<uint32_t>(renderTargetIndices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(_commandBuffer);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = renderTargets[imageIndex].image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(_commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkRenderPassBeginInfo presentRenderPassInfo{};
        presentRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        presentRenderPassInfo.renderPass = renderPass;
        presentRenderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        presentRenderPassInfo.renderArea.offset = {0, 0};
        presentRenderPassInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearColor = {{{(float)(0x4A) / 255.f, (float)(0x41) / 255.f, (float)(0x2A) / 255.f, 1.0f}}};
        presentRenderPassInfo.clearValueCount = 1;
        presentRenderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(_commandBuffer, &presentRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, presentPipeline.getPipeline());


        VkViewport presentViewport{};
        presentViewport.x = 0.0f;
        presentViewport.y = 0.0f;
        presentViewport.width = static_cast<float>(swapChainExtent.width);
        presentViewport.height = static_cast<float>(swapChainExtent.height);
        presentViewport.minDepth = 0.0f;
        presentViewport.maxDepth = 1.0f;
        vkCmdSetViewport(_commandBuffer, 0, 1, &presentViewport);

        VkRect2D presentScissor{};

        presentScissor.offset = {0, 0};
        presentScissor.extent = swapChainExtent;
        vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);

        VkBuffer presentVertexBuffers[] = {presentVertexBuffer.buffer};

        vkCmdBindVertexBuffers(_commandBuffer, 0, 1, presentVertexBuffers, offsets);

        vkCmdBindIndexBuffer(_commandBuffer, presentIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, presentPipeline.getPipelineLayout(), 0, 1, &presentDescriptorSets[currentFrame], 0, nullptr);

        vkCmdDrawIndexed(_commandBuffer, static_cast<uint32_t>(presentIndices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(_commandBuffer);

        if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    // Sync objects

    void createSyncObjects()
    {
        imageAvailableSemaphores.resize(framesInFlight);
        renderFinishedSemaphores.resize(framesInFlight);
        inFlightFences.resize(framesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < framesInFlight; i++)
        {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
            {

                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

private: // Drawing logic (actions ran every frame)
    void drawFrame()
    {
        VkFence &inFlightFence = inFlightFences[currentFrame];
        VkSemaphore &imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
        VkSemaphore &renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
        VkCommandBuffer &commandBuffer = commandBuffers[currentFrame];

        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        vkResetFences(device, 1, &inFlightFence);

        vkResetCommandBuffer(commandBuffer, 0);

        recordCommandBuffer(commandBuffer, imageIndex);

        updateUniformBuffer(currentFrame);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;


        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % framesInFlight;
    }

    void updateUniformBuffer(uint32_t currentImage) {

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));        
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(uniformBuffers[currentImage].mapped, &ubo, sizeof(ubo));

        
    }

private: // Vulkan Utils
    std::vector<const char *> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            extensions.emplace_back(glfwExtensions[i]);
        }

// Required for Mac OS compatibility
#ifdef _GLFW_COCOA
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        (void)pUserData;

        switch (messageSeverity)
        {
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

        switch (messageType)
        {
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

    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice _physicalDevice)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    static std::vector<char> readShader(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

private: // Vulkan checks
    void checkRequiredInstanceExtensions(const std::vector<const char *> &requiredExtensions)
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "Available extensions:" << std::endl;

        for (const auto &extension : extensions)
        {
            std::cout << '\t' << extension.extensionName << std::endl;
        }

        for (const auto &requiredExtension : requiredExtensions)
        {
            if (std::find_if(extensions.begin(), extensions.end(), [requiredExtension](const VkExtensionProperties &extension)
                             { return strcmp(extension.extensionName, requiredExtension) == 0; }) == extensions.end())
            {
                throw std::runtime_error("Required extension not found: " + std::string(requiredExtension));
            }
        }
    }

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::cout << "Available layers:" << std::endl;

        for (const auto &layer : availableLayers)
        {
            std::cout << '\t' << layer.layerName << std::endl;
        }

        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    bool checkDeviceExtensions(VkPhysicalDevice _physicalDevice)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

private:
// Rendering
    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    QueueFamilyIndices queueFamilyIndices;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue computeQueue;
    VkQueue transferQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;
    VkDescriptorSetLayout renderDescriptorSetLayout, presentDescriptorSetLayout;
    PresentPipeline presentPipeline;
    RenderPipeline renderPipeline; 
    VkDescriptorPool renderDescriptorPool;
    VkDescriptorPool presentDescriptorPool;

    std::vector<VkDescriptorSet> 
        renderDescriptorSets = {},
        presentDescriptorSets = {}
    ;
    VkCommandPool graphicsCommandPool, transferCommandPool;
    VmaAllocator allocator;
    VertexBuffer renderVertexBuffer;
    IndexBuffer renderIndexBuffer;

    VertexBuffer presentVertexBuffer;
    IndexBuffer presentIndexBuffer;

    std::vector<UniformBuffer> uniformBuffers;
    std::vector<RenderTarget> renderTargets;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t framesInFlight = 0;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

private: // Application
    std::chrono::steady_clock::time_point previousTime;
    typedef std::chrono::duration<float> duration;
    uint64_t frameCount = 0;
    float frameTime = 0.0;
};

// EndRegion Vulkan

int main()
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}