#include "app_pch.h"
#include "UILayer.h"

#include <stdlib.h>

void UILayer::OnAttach()
{
	_INFO("Initialised UI");
}

void UILayer::OnDetach()
{
}

void UILayer::OnUpdate(float dt)
{
	TextPrinter::Print("Client: ", TextColor::Blue);

	std::string input;
	std::getline(std::cin, input);

	//get user input

	//process input
	if (input == "/quit") {
	
	}

	//send message to sv

	//receive message from sv

	//process message

	m_ScreenInfo.EmplaceBack(Line::Type::Client, input);

	m_ScreenInfo.EmplaceBack(Line::Type::Server, "ok");
}

void UILayer::OnUIRender()
{
	ClearScreen();
	for (auto& line : m_ScreenInfo) 
	{
		line.Display();
	}
}

void UILayer::ClearScreen() const {
	system("CLS");
	fflush(stdout);
}
