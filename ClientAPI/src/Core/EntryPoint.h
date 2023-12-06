#pragma once

#include "Application.h"
#include "Config.h"
#include "Log.h"

extern Application* CreateApplication();

int main(int argc, char** argv)
{
	Log::Init();
	__INFO("Initialised Log!");

	Application* app = CreateApplication();
	__INFO("Application Created Successfully!");

	app->Run();

	delete app;
	__INFO("Close Application!\n");

	return 0;
}
