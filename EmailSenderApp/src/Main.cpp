#include "app_pch.h"
#include <Client.h>
#include <Core/EntryPoint.h>

#include "EmailSender/UILayer.h"

class EmailSender : public Application 
{
public:
	EmailSender() 
	{
		PushOverlay(new UILayer);
	}

	~EmailSender() {}

private:

};

Application* CreateApplication() {
	return new EmailSender();
}