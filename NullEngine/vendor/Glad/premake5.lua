project "Glad"
	kind "StaticLib"
	staticruntime "On"
	language "C"


	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

   includedirs
   {
		"include"
   }


	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		symbols "On"
		optimize "speed"		
		buildoptions "/MTd"
		runtime "Debug"		

	filter "configurations:Release"
		symbols "On"
		optimize "speed"
		runtime "Release"

	filter "configurations:Dist"
		symbols "On"
		optimize "speed"
		runtime "Release"
