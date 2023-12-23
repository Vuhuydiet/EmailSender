#pragma once

#include "Application.h"
#include "Config.h"
#include "Log.h"

extern Application* CreateApplication();

int main(int argc, char** argv)
{
	Log::Init();

	__INFO("Start of Program!");
	TextPrinter::Init();

	bool isRestart = false;
	do {
		Application* app = CreateApplication();
		__INFO("Application Created Successfully!");

		app->Run();

		isRestart = app->IsRestart();

		delete app;
		__INFO("Application Closed!");
	} while (isRestart);
	
	__INFO("End of Program!\n");

	return 0;
}
