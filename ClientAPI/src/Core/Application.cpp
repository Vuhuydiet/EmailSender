#include "pch.h"
#include "Application.h"

#include "Renderer/TextPrinter.h"

#include "Socket.h"

Application* Application::s_Instance = nullptr;

Application::Application()
	: m_LastFrameTime(0.0f),
	m_Running(true)
{
	ASSERT(!s_Instance, "Already have an instance for Application class!\n");
	s_Instance = this;

	TextPrinter::Init();
	Socket::Init();
}

Application::~Application()
{
	Socket::Shutdown();

	s_Instance = nullptr;
}

void Application::Run()
{
	while (IsRunning())
	{
		float currentTime = m_Clock.GetSeconds();
		float dt = currentTime - m_LastFrameTime;
		m_LastFrameTime = currentTime;

		for (auto& layer : m_LayerStack)
		{
			layer->OnUpdate(dt);
		}

		for (auto& layer : m_LayerStack) 
		{
			layer->OnUIRender();
		}
	}
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

