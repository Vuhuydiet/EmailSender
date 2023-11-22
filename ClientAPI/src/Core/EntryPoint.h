#pragma once

#include "Application.h"
#include "Config.h"
#include "Log.h"

extern Application* CreateApplication();

int main(int argc, char** argv)
{
	Log::Init();
	_INFO("Initiated Log!");

	Application* app = CreateApplication();
	_INFO("Application Created!");

	app->Run();

	delete app;
	_INFO("Close Application!");

	return 0;
}
