project "Glad"
	kind "StaticLib"
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
		staticruntime "On"
		buildoptions "/MT"

	filter "configurations:Debug"
		symbols "On"
		optimize "speed"		
		buildoptions "/MT"
		runtime "Debug"		

	filter "configurations:Release"
		symbols "On"
		optimize "speed"
		buildoptions "/MT"
		runtime "Release"

	filter "configurations:Dist"
		symbols "On"
		optimize "speed"
		buildoptions "/MT"
		runtime "Release"
