
project "EmailSenderApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "app_pch.h"
	pchsource "src/app_pch.cpp"


	defines
	{
		"_APP",
		"_CRT_SECURE_NO_WARNINGS",
		"YAML_CPP_STATIC_DEFINE"
	}

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/ClientAPI/src",
		
		"%{wks.location}/ClientAPI/vendor/spdlog/include",
		"%{wks.location}/ClientAPI/vendor/yaml-cpp/include"
	}

	links
	{
		"ClientAPI",

		"yaml-cpp"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}
		
		prebuildcommands
		{
		}
		
	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "_DIST"
		runtime "Release"
		optimize "on"