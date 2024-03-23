#include <Engine.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <optional>
#include <set>

// Region Vulkan

const int WIDTH = 800;
const int HEIGHT = 600;


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> computeFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && computeFamily.has_value();
    }
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

        pickPhysicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {

        vkDestroyDevice(device, nullptr);

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
        __pragma(warning(push))
        __pragma(warning(disable:4127))
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        __pragma(warning(pop))

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
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
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

    // Queue Families

    std::pair<std::vector<VkQueueFamilyProperties>, QueueFamilyIndices> findQueueFamilies(VkPhysicalDevice _physicalDevice) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        std::optional<int> graphicsFamily = std::nullopt;
        std::optional<int> presentFamily = std::nullopt;
        std::optional<int> computeFamily = std::nullopt;
        std::optional<int> graphicsAndComputeFamily = std::nullopt;
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
            // vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

        
            i++;
        }

        if (computeOnlyQueue.has_value()) {
            indices.computeFamily = computeOnlyQueue;
        } else if (graphicsAndComputeFamily.has_value()) {
            indices.computeFamily = graphicsAndComputeFamily;
        } else if (computeFamily.has_value()) {
            indices.computeFamily = computeFamily;
        } 

        if (graphicsOnlyQueue.has_value()) {
            indices.graphicsFamily = graphicsOnlyQueue;
        } else if (graphicsAndComputeFamily.has_value()) {
            indices.graphicsFamily = graphicsAndComputeFamily;
        } else if (graphicsFamily.has_value()) {
            indices.graphicsFamily = graphicsFamily;
        }

        indices.presentFamily = presentFamily;

        

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

        return pair.second.isComplete();
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

        if (indices.graphicsFamily == indices.graphicsFamily) {
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

        createInfo.enabledExtensionCount = 0;

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

private:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue computeQueue;
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