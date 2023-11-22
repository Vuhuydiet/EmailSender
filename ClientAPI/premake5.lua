project "ClientAPI"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	defines
	{
		"_LIB",

		"_CRT_SECURE_NO_WARNINGS"
	}

	files
    {
        "src/**.h",
        "src/**.cpp"
    }

	includedirs
	{
		"src",
		"vendor/spdlog/include"
	}

	libdirs
	{
	}

	links
	{
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
