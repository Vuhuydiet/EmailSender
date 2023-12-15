#pragma once

#include "Clock.h"
#include "LayerStack.h"

#include "Config.h"
#include<thread>

int main(int argc, char** argv);

class Application
{
public:
	Application();
	virtual ~Application();

	bool IsRunning() const { return m_Running; }
	void Close() { m_Running = false; }

	static Application& Get() { return *s_Instance; }

protected:
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

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

