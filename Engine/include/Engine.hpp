#ifndef ENGINE_HPP
#define ENGINE_HPP
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan_memory_allocator/vk_mem_alloc.h>

namespace Engine {
    int add(int a, int b);
}

#endif