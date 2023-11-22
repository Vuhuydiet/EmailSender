#pragma once

#include "Clock.h"
#include "LayerStack.h"

#include "Config.h"

int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	bool IsRunning() const { return m_Running; }
	void Close() { m_Running = false; }

	static Application& Get() { return *s_Instance; }

private:
	void Run();
	bool m_Running;

	Clock m_Clock;
	float m_LastFrameTime;
private:
	LayerStack m_LayerStack;

private:
	static Application* s_Instance;
	friend int ::main(int argc, char** argv);
};

