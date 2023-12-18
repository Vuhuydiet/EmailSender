#pragma once

#include <filesystem>

#include <Client.h>

class AutoloadLayer: public Layer {
public:
	virtual void OnUpdate(float dt) override;

private:
	Ref<Socket> m_Socket = nullptr;
	float m_TimeCounter = FLT_MAX;
};