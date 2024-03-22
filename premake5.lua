---@diagnostic disable: undefined-global
-- Premake 5 workspace

workspace "VulkanWorkspace"

    -- toolset "clang"

    local vulkan_sdk = os.getenv("VULKAN_SDK")
    local VULKAN_SDK_INCLUDE = vulkan_sdk .. "/include"
    local VULKAN_SDK_LIB = vulkan_sdk .. "/lib"

    

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
        flags { "FatalWarnings" }
        warnings "Everything"

    project "Engine"
        kind "StaticLib"
        location "Engine"
        language "C++"
        cppdialect "C++17"
        targetdir "build/bin/%{cfg.buildcfg}"
        objdir "build/obj/%{cfg.buildcfg}"
        files { "Engine/**.hpp", "Engine/**.cpp" }
        includedirs { "Engine/include", VULKAN_SDK_INCLUDE }
        libdirs { VULKAN_SDK_LIB }
        links { "vulkan-1" }
        flags { "FatalWarnings" }
        warnings "Everything"

    --    links { "glfw", "glad", "stb_image" }

    