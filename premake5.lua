---@diagnostic disable: undefined-global
-- Premake 5 workspace

-- TODO: It has occured to me that if i ever want to support wayland I'll have to switch to CMake

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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
        targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
        objdir ("build/obj/" .. outputdir .. "/%{prj.name}")

        files { "Main/include/**.hpp", "Main/src/**.cpp" }
        includedirs { "Main/include", "Engine/include", VULKAN_SDK_INCLUDE, "Engine/vendor/glfw/include", "Engine/vendor/glm/include" }
        flags { "FatalWarnings" }
        warnings "Extra"
        links { "Engine", "GLFW" }
        libdirs { VULKAN_SDK_LIB }

        filter "system:linux"
            links { "vulkan" }

        filter "system:windows"
            links { "vulkan-1" }

    project "Engine"
        kind "StaticLib"
        location "Engine"
        language "C++"
        cppdialect "C++17"
        targetdir ("build/bin/" .. outputdir .. "/%{prj.name}")
        objdir ("build/obj/" .. outputdir .. "/%{prj.name}")
        files { "Engine/src/**.hpp", "Engine/src/**.cpp", "Engine/include/**.hpp" }
        includedirs { "Engine/include", VULKAN_SDK_INCLUDE, "Engine/vendor/glfw/include", "Engine/vendor/glm/include" }

        flags { "FatalWarnings" }
        warnings "Extra"

        links { "GLFW"
        --"glad", "stb_image" 
        }
        libdirs { VULKAN_SDK_LIB }

        filter "system:linux"
            links { "vulkan" }

        filter "system:windows"
            links { "vulkan-1" }





include "Engine/vendor/glfw"
