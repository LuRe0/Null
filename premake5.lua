workspace "NullEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includeDir = {}

includeDir["GLFW"] = "NullEngine/vendor/GLFW/include" 

-- Include the GLFW project
include "NullEngine/vendor/GLFW"

project "NullEngine"
    location "NullEngine"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

    pchheader "stdafx.h"
    pchsource "NullEngine/src/stdafx.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
         "%{prj.name}/vendor/spdlog/include",
         "%{includeDir.GLFW}"
        --  "%{prj.name}/vendor/GLFW/include/GLFW"
    }

    defines
    {
        "NLE_PLATFORM_WINDOWS",
        "NLE_BUILD_DLL"
    }

        
    links 
    {
        "GLFW",
        "opengl32.lib"
    }

    postbuildcommands
    {
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/sandbox")
    }

    filter "system.windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        filter "configurations:Debug"
            defines "NLE_DEBUG"
            symbols "On"

        filter "configurations:Release"
            defines "NLE_RELEASE"
            symbols "On"

        filter "configurations:Dist"
            defines "NLE_DIST"
            symbols "On"


    

project "Sandbox" 
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
         "NullEngine/vendor/spdlog/include",
        --  "NullEngine/vendor/GLFW/include/GLFW",
         "NullEngine/src",
         "%{includeDir.GLFW}"
         
    }

    defines
    {
        "NLE_PLATFORM_WINDOWS"
    }

    
    links 
    {
        "NullEngine"
    }

    filter "system.windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"

        filter "configurations:Debug"
            defines "NLE_DEBUG"
            symbols "On"

        filter "configurations:Release"
            defines "NLE_RELEASE"
            symbols "On"

        filter "configurations:Dist"
            defines "NLE_DIST"
            symbols "On"

