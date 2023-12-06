#pragma once
#include <Client.h>

#include "ScreenInfo.h"

class CommandLineUILayer : public Layer 
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnUIRender() override;

private:
	enum class State { AppInteracting, Sending };

private:
	void ClearScreen();

private:
	ScreenInfo m_ScreenInfo;
	State m_State = State::AppInteracting;
	std::string m_InputBuffer;

private:
	Ref<Socket> m_Socket;
};