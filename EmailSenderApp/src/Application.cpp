#include "app_pch.h"
#include <Client.h>
#include <Core/EntryPoint.h>

#include "CommandLineUI/CommandLineUILayer.h"
#include "SelectionUI/UILayer.h"
#include "AutoloadLayer/AutoloadLayer.h"

class EmailSender : public Application 
{
public:
	EmailSender() 
	{
		// PushOverlay(new CommandLineUILayer());
		PushOverlay(new UILayer());
		PushLayer(new AutoloadLayer());
	}

	~EmailSender() {}

private:
};

Application* CreateApplication() {
	return new EmailSender();
}