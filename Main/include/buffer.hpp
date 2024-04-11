#pragma once

#include "Engine.hpp"
#include <vector>
#include <set>
#include <stdexcept>

struct Buffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
};

struct VertexBuffer : public Buffer
{
};

struct StagingBuffer : public Buffer
{
};

struct IndexBuffer : public Buffer
{
};

struct UniformBuffer : public Buffer
{
    void *mapped;
};


struct CreateBufferInfo
{
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VmaMemoryUsage memoryUsage;
    VmaAllocationCreateFlags flags = {};
    std::vector<uint32_t> queueFamilyIndices = {};
};


void createBuffer(CreateBufferInfo bufferCreateInfo,
                  VmaAllocator allocator,
                  VmaAllocation &allocation,
                  VkBuffer &buffer);