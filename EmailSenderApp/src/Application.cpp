#include "app_pch.h"
#include <Client.h>
#include <Core/EntryPoint.h>

#include "DefaultVars/Vars.h"
#include "Config/Config.h"
#include "UILayer/UILayer.h"
#include "AutoloadLayer/AutoloadLayer.h"

class EmailSender : public Application 
{
public:
	EmailSender()
	{
		CreateDirsIfNotExist({ _DEFAULT_HOST_MAILBOX_DIR, _DEFAULT_CONFIG_DIR });

		auto& config = Config::Get();
		config.Init(_DEFAULT_APP_CONFIG_FILEPATH);
		config.Load();


		PushOverlay(new UILayer());
		PushLayer(new AutoloadLayer());
	}

	~EmailSender() {}

private:

};

Application* CreateApplication() {
	return new EmailSender();
}