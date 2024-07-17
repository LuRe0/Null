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
includeDir["Glad"] = "NullEngine/vendor/Glad/include" 
includeDir["JSON"] = "NullEngine/vendor/JSON/include" 
includeDir["glm"] = "NullEngine/vendor/glm" 
includeDir["ImGui"] = "NullEngine/vendor/imgui" 
includeDir["stb"] = "NullEngine/vendor/stb/stb" 


-- Include the GLFW project
include "NullEngine/vendor/GLFW"
include "NullEngine/vendor/Glad"
include "NullEngine/vendor/imgui"

project "NullEngine"
    location "NullEngine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"

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
         "%{includeDir.GLFW}",
         "%{includeDir.Glad}",
         "%{includeDir.JSON}",
         "%{includeDir.glm}",
         "%{includeDir.stb}",
         "%{includeDir.ImGui}"
    }

    defines
    {
        "NLE_PLATFORM_WINDOWS",
        "NLE_BUILD_DLL",
        "GLFW_INCLUDE_NONE"
    }

        
    links 
    {
        "GLFW",
        "Glad",
        "ImGui",
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
            buildoptions "/MDd"
            symbols "On"

        filter "configurations:Release"
            defines "NLE_RELEASE"
            buildoptions "/MD"
            symbols "On"

        filter "configurations:Dist"
            defines "NLE_DIST"
            buildoptions "/MT"
            symbols "On"


    

project "Sandbox" 
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"


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
         "NullEngine/src",
         "%{includeDir.GLFW}",
         "%{includeDir.Glad}",
         "%{includeDir.JSON}",
         "%{includeDir.glm}"
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

