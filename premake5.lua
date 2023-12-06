workspace "EmailSender"
	architecture "x64"
	startproject "EmailSenderApp"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "ClientAPI/vendor/yaml-cpp"
group ""

group "Core"
	include "ClientAPI"
	include "EmailSenderApp"
group ""
