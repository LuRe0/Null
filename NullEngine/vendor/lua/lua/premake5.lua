project "Lua"
	kind "StaticLib"
	language "C"
	
    staticruntime "on"

    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputDir .. "/%{prj.name}")


	files
	{
        "src/**.h",
        "src/**.c"
	}
	


    includedirs
    {
        "src"
    }

	filter "system:windows"
		systemversion "latest"
		staticruntime "on"
		cppdialect "C++17"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

    filter "configurations:Dist"
		runtime "Release"
		optimize "on"
        symbols "off"
