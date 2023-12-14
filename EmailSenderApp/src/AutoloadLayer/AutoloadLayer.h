#pragma once

#include <filesystem>

#include <Client.h>

class AutoloadLayer: public Layer {
public:
	virtual void OnUpdate(float dt) override;

private:
	void ReloadConfigAttributes();
private:
	Ref<Socket> m_Socket = nullptr;
	float m_TimeCounter = 0.0f;

	// To be read in config file
	float m_AutoloadDelay = 0.0f;
	std::string m_Username;
	std::string m_Password;
};