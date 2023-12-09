#include "app_pch.h"
#include "UILayer.h"

// TEMP
#include <WinSock2.h>

void UILayer::OnAttach()
{
}

void UILayer::OnDetach()
{
	//m_Socket->Disconnect();
}

void UILayer::OnUpdate(float dt)
{

	// TEMP
	m_Socket = Socket::Create({ AF_INET, SOCK_STREAM, IPPROTO_TCP });
	m_Socket->Connect("127.0.0.1", 2500);
	m_Socket->Receive();

	std::string inp;
	SentMail mail;

	TextPrinter::Print("Sender: ");
	std::getline(std::cin, inp);
	mail.Sender = inp;

	TextPrinter::Print("To: ");
	std::getline(std::cin, inp);
	mail.AddTo(inp);
	
	TextPrinter::Print("Cc: ");
	std::getline(std::cin, inp);
	mail.AddCc(inp);
	
	TextPrinter::Print("Bcc: ");
	std::getline(std::cin, inp);
	mail.AddBcc(inp);

	TextPrinter::Print("Subject: ");
	std::getline(std::cin, inp);
	mail.Subject = inp;

	TextPrinter::Print("Content: ");
	std::getline(std::cin, inp);
	mail.Content.push_back(inp);


	SMTP::SendMail(m_Socket, mail);
	TextPrinter::Print("Done sending email!\n", TextColor::Green);

	m_Socket = nullptr;
	
	TextPrinter::Print("/quit to quit\n");
	std::getline(std::cin, inp);
	if (inp == "/quit")
		Application::Get().Close();
}

void UILayer::OnUIRender()
{
}
