---@diagnostic disable: undefined-global
-- Premake 5 project

project "VulkanMemoryAllocator"
    kind "StaticLib"
    location "."
    language "C++"
    cppdialect "C++17"
	warnings "off"
	targetdir (_WORKING_DIR .. "/build/bin/" .. outputdir )
	objdir (_WORKING_DIR .. "/build/obj/" .. outputdir )

    files {
        "include/vulkan_memory_allocator/vk_mem_alloc.h",

        "src/vk_mem_alloc.cpp",
    }

    includedirs {
        VULKAN_SDK_INCLUDE,
        "include",
    }

    libdirs { VULKAN_SDK_LIB }

    filter "system:linux"
        links { "vulkan" }

    filter "system:windows"
        links { "vulkan-1" }