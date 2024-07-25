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
includeDir["Box2D"] = "NullEngine/vendor/Box2D/include" 
includeDir["glm"] = "NullEngine/vendor/glm" 
includeDir["ImGui"] = "NullEngine/vendor/imgui" 
includeDir["stb"] = "NullEngine/vendor/stb/stb" 

group "Depenencies"
    include "NullEngine/vendor/GLFW"
    include "NullEngine/vendor/Glad"
    include "NullEngine/vendor/imgui"
    include "NullEngine/vendor/Box2D"
group ""

project "NullEngine"
    location "NullEngine"
    kind "StaticLib"
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
         "%{includeDir.Box2D}",
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
        "Box2D",
        "ImGui",
        "opengl32.lib"
    }

    postbuildcommands
    {
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/sandbox"),
        ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputDir .. "/NullEditor")
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
    -- staticruntime "on"

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

project "NullEditor" 
    location "NullEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    -- staticruntime "on"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

    links 
    {
        "NullEngine"
    }
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
         "NullEngine/vendor/spdlog/include",
         "NullEngine/src",
         "NullEditor/src",
         "%{includeDir.GLFW}",
         "%{includeDir.Glad}",
         "%{includeDir.ImGui}",
         "%{includeDir.JSON}",
         "%{includeDir.glm}"
    }

    defines
    {
        "NLE_PLATFORM_WINDOWS"
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

