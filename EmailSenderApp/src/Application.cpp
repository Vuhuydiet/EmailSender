#include "app_pch.h"
#include <Client.h>
#include <Core/EntryPoint.h>

#include "CommandLineUI/CommandLineUILayer.h"

class EmailSender : public Application 
{
public:
	EmailSender() 
	{
		PushOverlay(new CommandLineUILayer);
	}

	~EmailSender() {}

private:
};

Application* CreateApplication() {
	return new EmailSender();
}