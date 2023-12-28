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

	void Restart() { m_Running = false, m_Restart = true; }
	bool IsRestart() const { return m_Restart; }

	static Application& Get() { return *s_Instance; }

protected:
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

private:
	void Run();
	bool m_Running = true;

	bool m_Restart = false;

	Clock m_Clock;
	float m_LastFrameTime;
private:
	LayerStack m_LayerStack;

private:
	static Application* s_Instance;
	friend int ::main(int argc, char** argv);
};

