#include "queue_families.hpp"
#include <algorithm>
#include <iostream>

namespace __QueueFamilies_Impl
{

    QueueFamilyOptions findQueueFamily(
        const std::vector<VkQueueFamilyProperties> &queueFamilies,
        std::function<bool(VkQueueFamilyProperties, uint32_t index)> matchOnlyRequiredFlag,
        std::function<bool(VkQueueFamilyProperties, uint32_t index)> matchOptionalFlag)
    {

        std::vector<QueueFamilyIndex> onlyRequiredFlagQueue;
        std::vector<QueueFamilyIndex> requiredAndOptionalFlagQueue;

        for (size_t i = 0; i < queueFamilies.size(); i++)
        {
            auto queueFamily = queueFamilies[i];
            if (matchOnlyRequiredFlag(queueFamily, (uint32_t)i) && !matchOptionalFlag(queueFamily, (uint32_t)i))
            {
                onlyRequiredFlagQueue.push_back(
                    QueueFamilyIndex{
                        (uint32_t)i,
                        queueFamily.queueCount,
                    });
            }

            if (matchOptionalFlag(queueFamily, (uint32_t)i) && matchOnlyRequiredFlag(queueFamily, (uint32_t)i))
            {
                requiredAndOptionalFlagQueue.push_back(
                    QueueFamilyIndex{
                        (uint32_t)i,
                        queueFamily.queueCount,
                    });
            }
        }

        return QueueFamilyOptions{
            onlyRequiredFlagQueue,
            requiredAndOptionalFlagQueue};
    }

 

    void tryIncrement(QueueFamilyIndices& indices, std::vector<QueueFamilyFindData*>& findData)
    {

        if (indices.isComplete()) {
            return;
        }

        for(auto data : findData) {
            if(data->canIncrement() && !data->output.has_value()) {
                data->queueFamilyIndex++;

                tryIndices(indices, findData);

                if(indices.isComplete()) 
                    return;
                else 
                    data->queueFamilyIndex--;

            }
        }

        
    };

    uint32_t countEqual(QueueFamilyIndex a, std::vector<QueueFamilyIndex> b)
    {
        return (uint32_t)std::count_if(b.begin(), b.end(), [&](QueueFamilyIndex c)
                             { return a.family == c.family; });
    };


    void tryIndices(QueueFamilyIndices& indices, std::vector<QueueFamilyFindData*>& findData)
    {
        if (indices.isComplete()) {
            return;
        }

        for (auto data : findData)
        {
            if( data->output.has_value() ) {
                continue;
            }

            if (data->queueFamilyOptions().size() == 0)
            {
                return;
            }

            auto count = countEqual(data->getQueueFamily(), data->getIncrementIfSame());

            if(data->getQueueFamily().queueCount > count) {
                
                uint32_t queueIndex = 0;
                for(auto noIncrement : data->noIncrementIfSame) {
                    if(!noIncrement->output.has_value() && noIncrement->getFamily() == data->getFamily()) {
                        noIncrement->output = QueueFamilyIndex {
                            data->getFamily(),
                            data->getQueueFamily().queueCount, 
                            queueIndex                        
                        };
                    }
                }

                data->output = QueueFamilyIndex {
                    data->getFamily(),
                    data->getQueueFamily().queueCount, 
                    queueIndex++                        
                };

                for(auto increment : data->incrementIfSame) {
                    if(!increment->output.has_value() && increment->getFamily() == data->getFamily()) {
                        increment->output = QueueFamilyIndex {
                            data->getFamily(),
                            data->getQueueFamily().queueCount, 
                            queueIndex++                        
                        };
                    }
                }

            }
            else {
                tryIncrement( indices, findData );


            }        
        }

    }

    QueueFamilyIndices findQueueFamilies(
        VkPhysicalDevice device,
        VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


        auto checkPresentSupport = [&](uint32_t i)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            return presentSupport;
        };

        std::vector<QueueFamilyFindData*> findData = {};

        QueueFamilyFindData graphicsFamily = {
            "Graphics",
            findQueueFamily(
                queueFamilies,
                [](VkQueueFamilyProperties queueFamily, uint32_t index)
                {
                    (void)index;
                    return queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
                },
                [&](VkQueueFamilyProperties queueFamily, uint32_t index)
                {
                    (void)index;
                    return queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
                }),
            indices.graphicsFamily
        }; 

        QueueFamilyFindData computeFamily = {
            "Compute",
            findQueueFamily(
                queueFamilies,
                [](VkQueueFamilyProperties queueFamily, uint32_t index)
                {
                    (void)index;
                    return queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT;
                },
                [&](VkQueueFamilyProperties queueFamily, uint32_t index)
                {
                    return queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT || queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT ||  checkPresentSupport(index);
                    ;
                }),
            indices.computeFamily
        };

        QueueFamilyFindData transferFamily = {
            "Transfer",
            findQueueFamily(
                queueFamilies,
                [](VkQueueFamilyProperties queueFamily, uint32_t)
                {
                    return queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT;
                },
                [&](VkQueueFamilyProperties queueFamily, uint32_t index)
                {
                    return queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT || queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT || checkPresentSupport(index);
                    ;
                }),
            indices.transferFamily
        };

        QueueFamilyFindData presentFamily = {
            "Present",
            findQueueFamily(
                queueFamilies,
                [&](VkQueueFamilyProperties, uint32_t index)
                {
                    return checkPresentSupport(index);
                },
                [&](VkQueueFamilyProperties queueFamily, uint32_t)
                {
                    return queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT || queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT;
                }),
            indices.presentFamily
        };

        graphicsFamily.incrementIfSame = {&computeFamily, &transferFamily};
        graphicsFamily.noIncrementIfSame = {&presentFamily};

        computeFamily.incrementIfSame = {&graphicsFamily, &transferFamily, &presentFamily};
        
        transferFamily.incrementIfSame = {&graphicsFamily, &computeFamily, &presentFamily};

        presentFamily.incrementIfSame = {&computeFamily, &transferFamily};
        presentFamily.noIncrementIfSame = {&graphicsFamily};

        findData = {&graphicsFamily, &computeFamily, &transferFamily, &presentFamily};


        tryIndices(indices, findData);
        
        return indices;
    }

}

QueueFamilyIndices findQueueFamilies(
    VkPhysicalDevice device,
    VkSurfaceKHR surface)
{
    return __QueueFamilies_Impl::findQueueFamilies(device, surface);
}