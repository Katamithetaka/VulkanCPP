#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <vector>
#include <optional>
#include <Engine.hpp>

struct QueueFamilyIndices;

namespace __QueueFamilies_Impl
{
    struct QueueFamilyIndex
    {
        uint32_t family;
        uint32_t queueCount;
        uint32_t queueIndex = 0;
    };

    struct QueueFamilyOptions
    {
        std::vector<QueueFamilyIndex> onlyMatchRequiredQueues;
        std::vector<QueueFamilyIndex> matchWithOptionalQueues;
    };

    struct Indices
    {
        uint32_t graphicsFamilyIndex = 0,
                 computeFamilyIndex = 0,
                 transferFamilyIndex = 0,
                 presentFamilyIndex = 0;
    };

    struct QueueFamilyFindData {
        std::string name = "";
        QueueFamilyOptions options = {};
        std::optional<QueueFamilyIndex>& output;
        std::vector<QueueFamilyFindData*> incrementIfSame = {};
        std::vector<QueueFamilyFindData*> noIncrementIfSame = {};
        uint32_t queueFamilyIndex = 0;

        QueueFamilyIndex& getQueueFamily() {
            return queueFamilyOptions().at(queueFamilyIndex);
        }

        std::vector<QueueFamilyIndex> getIncrementIfSame() {
            std::vector<QueueFamilyIndex> returnVal = {};
            for (auto data : incrementIfSame)
            {
                returnVal.push_back(data->getQueueFamily());
            }
            return returnVal;
        }

        std::vector<QueueFamilyIndex>& queueFamilyOptions() {
            return options.onlyMatchRequiredQueues.size() ? options.onlyMatchRequiredQueues : options.matchWithOptionalQueues;
        }

        uint32_t getFamily() {
            return getQueueFamily().family;
        }

        bool canIncrement() {
            return options.matchWithOptionalQueues.size() > queueFamilyIndex + 1;
        }

    };

    QueueFamilyOptions findQueueFamily(
        const std::vector<VkQueueFamilyProperties> &queueFamilies,
        std::function<bool(VkQueueFamilyProperties, uint32_t index)> matchOnlyRequiredFlag,
        std::function<bool(VkQueueFamilyProperties, uint32_t index)> matchOptionalFlag = [](VkQueueFamilyProperties, uint32_t)
        { return false; });

    QueueFamilyIndices findQueueFamilies(
        VkPhysicalDevice device,
        VkSurfaceKHR surface);

    void tryIncrement(QueueFamilyIndices& indices, std::vector<QueueFamilyFindData*>& findData);
    void tryIndices(QueueFamilyIndices& indices, std::vector<QueueFamilyFindData*>& findData);
    uint32_t countEqual(QueueFamilyIndex a, std::vector<QueueFamilyIndex> b);

} // namespace Impl

struct QueueFamilyIndices
{
    std::optional<__QueueFamilies_Impl::QueueFamilyIndex> graphicsFamily;
    std::optional<__QueueFamilies_Impl::QueueFamilyIndex> presentFamily;
    std::optional<__QueueFamilies_Impl::QueueFamilyIndex> computeFamily;
    std::optional<__QueueFamilies_Impl::QueueFamilyIndex> transferFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
    }

    std::vector<VkDeviceQueueCreateInfo> createQueueCreateInfos(float queuePriority = 1.0f)
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        auto _graphicsFamily = graphicsFamily.value();
        auto _presentFamily = presentFamily.value();
        auto _computeFamily = computeFamily.value();
        auto _transferFamily = transferFamily.value();

        bool presentFamilyAdded = false,
             computeFamilyAdded = false,
             transferFamilyAdded = false;

        std::vector<std::vector<float>> pQueuePriorities;
        {

            uint32_t queueCount = 1;

            if (_computeFamily.family == _graphicsFamily.family)
            {
                queueCount++;
                computeFamilyAdded = true;
            }

            if (_transferFamily.family == _graphicsFamily.family)
            {
                queueCount++;
                transferFamilyAdded = true;
            }

            if (_presentFamily.family == _graphicsFamily.family)
            {
                presentFamilyAdded = true;
            }

            // queue priorities
            std::vector<float> queuePriorities(queueCount, queuePriority);
            pQueuePriorities.push_back(queuePriorities);

            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = _graphicsFamily.family;
            queueCreateInfo.queueCount = queueCount;
            queueCreateInfo.pQueuePriorities = pQueuePriorities.back().data();
            queueCreateInfos.push_back(queueCreateInfo);
        }

        if (!computeFamilyAdded)
        {

            uint32_t queueCount = 1;

            if (_transferFamily.family == _computeFamily.family)
            {
                queueCount++;
                transferFamilyAdded = true;
            }

            if (_presentFamily.family == _computeFamily.family)
            {
                queueCount++;
                presentFamilyAdded = true;
            }

            // queue priorities
            std::vector<float> queuePriorities(queueCount, queuePriority);
            pQueuePriorities.push_back(queuePriorities);

            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = _computeFamily.family;
            queueCreateInfo.queueCount = queueCount;
            queueCreateInfo.pQueuePriorities = pQueuePriorities.back().data();
            queueCreateInfos.push_back(queueCreateInfo);
        }

        if (!transferFamilyAdded)
        {
            uint32_t queueCount = 1;

            if (_presentFamily.family == _transferFamily.family)
            {
                queueCount++;
                presentFamilyAdded = true;
            }

            // queue priorities
            std::vector<float> queuePriorities(queueCount, queuePriority);
            pQueuePriorities.push_back(queuePriorities);

            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = _transferFamily.family;
            queueCreateInfo.queueCount = queueCount;
            queueCreateInfo.pQueuePriorities = pQueuePriorities.back().data();
            queueCreateInfos.push_back(queueCreateInfo);
        }

        if (!presentFamilyAdded)
        {
            uint32_t queueCount = 1;

            // queue priorities
            std::vector<float> queuePriorities(queueCount, queuePriority);
            pQueuePriorities.push_back(queuePriorities);

            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = _presentFamily.family;
            queueCreateInfo.queueCount = queueCount;
            queueCreateInfo.pQueuePriorities = pQueuePriorities.back().data();
            queueCreateInfos.push_back(queueCreateInfo);
        }

        return queueCreateInfos;
    }
};

QueueFamilyIndices findQueueFamilies(
    VkPhysicalDevice device,
    VkSurfaceKHR surface);