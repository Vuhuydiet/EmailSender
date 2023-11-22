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

group "Core"
	include "ClientAPI"
	include "EmailSenderApp"
group ""
