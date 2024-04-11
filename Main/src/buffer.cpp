#include "buffer.hpp"

void createBuffer(CreateBufferInfo bufferCreateInfo,
                  VmaAllocator allocator,
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