---@diagnostic disable: undefined-global
-- Premake 5 workspace

workspace "VulkanWorkspace"

    -- toolset "clang"

    local vulkan_sdk = os.getenv("VULKAN_SDK")
    local VULKAN_SDK_INCLUDE = vulkan_sdk .. "/Include"
    local VULKAN_SDK_LIB = vulkan_sdk .. "/Lib"


    configurations { "Debug", "Release" }
    platforms { "x64" }
    location ""
    startproject "Main"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "platforms:x64"
        architecture "x86_64"

    filter {}

    project "Main"
        kind "ConsoleApp"
        location "Main"
        language "C++"
        cppdialect "C++17"
        targetdir "build/bin/%{cfg.buildcfg}"
        objdir "build/obj/%{cfg.buildcfg}"
        files { "Main/include/**.hpp", "Main/src/**.cpp" }
        includedirs { "Main/include", "Engine/include", VULKAN_SDK_INCLUDE }
        links { "Engine" }

    project "Engine"
        kind "StaticLib"
        location "Engine"
        language "C++"
        cppdialect "C++17"
        targetdir "build/bin/%{cfg.buildcfg}"
        objdir "build/obj/%{cfg.buildcfg}"
        files { "engine/**.hpp", "engine/**.cpp" }
        includedirs { "Engine/include", VULKAN_SDK_INCLUDE }
        libdirs { VULKAN_SDK_LIB }
        links { "vulkan-1" }
    --    links { "glfw", "glad", "stb_image" }

    