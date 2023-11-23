#include "app_pch.h"
#include "UILayer.h"

#include <stdlib.h>
#include "UserInput.h"

// TEMP
#include <WinSock2.h>

void UILayer::OnAttach()
{
	_INFO("Initialised UI");

	m_Socket = Socket::Create();
	m_Socket->Init(SocketProps(AF_INET, SOCK_STREAM, IPPROTO_TCP));

	// TEMP
	m_Socket->Connect("127.0.0.1", 2500);
}

void UILayer::OnDetach()
{
	m_Socket->Disconnect();
}

void UILayer::OnUpdate(float dt)
{
	TextPrinter::Print("Client: ", TextColor::Blue);

	// Get user input
	UserInput input = UserInput::Get();

	// Process user input
	bool callsv = false;
	std::string cmd = input.GetCommand();
	switch(input.GetType())
	{
	case UserInput::Type::AppCommand:
		m_ScreenInfo.EmplaceBack(Line::Type::Client, input.GetInput());
		if (cmd == "/quit")
		{
			Application::Get().Close();
		}
		else if (cmd == "/clr")
		{
			m_ScreenInfo.Clear();
		}
		else if (cmd == "/begin")
		{
			m_State = State::Sending;
			m_ScreenInfo.EmplaceBack(Line::Type::App, "Start collecting messages:");
		}
		else if (cmd == "/end")
		{
			m_State = State::AppInteracting;
			m_ScreenInfo.EmplaceBack(Line::Type::App, "End collecting messages.");
		}
		return;
	case UserInput::Type::ServerCommand:
		if (cmd == "/send")
		{
			m_ScreenInfo.EmplaceBack(Line::Type::Server, "Sending message...");
			callsv = true;
		}
		break;
	case UserInput::Type::Message:
		Line::Type ty;
		if (m_State == State::AppInteracting)
			ty = Line::Type::Client;
		else
		{
			ty = Line::Type::None;
			m_InputMessages += input.GetMsg() + '\n';
		}
		m_ScreenInfo.EmplaceBack(ty, input.GetMsg());
		break;
	}

	if (!callsv)
		return;

	// Send message to server if user input is server command
	m_Socket->Send(m_InputMessages);
	m_InputMessages.clear();

	// Receive server response
	std::string serverResponse = m_Socket->Receive();
  
	// Process server response

	// Add Server response to screen
	m_ScreenInfo.EmplaceBack(Line::Type::Server, serverResponse);
}

void UILayer::OnUIRender()
{
	ClearScreen();

	for (auto& line : m_ScreenInfo) 
	{
		line.Display();
	}
}

void UILayer::ClearScreen()
{
	system("CLS");
	fflush(stdout);
}

void UILayer::GetMessages(std::string& msg)
{
	std::string line;
	while (std::getline(std::cin, line) && line != "/end") {
		msg += line;
	}
}