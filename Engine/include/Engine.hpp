#ifndef ENGINE_HPP
#define ENGINE_HPP
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan_memory_allocator/vk_mem_alloc.h>

namespace Engine {
    int add(int a, int b);
}

#endif