#include "app_pch.h"
#include "UILayer.h"
#include "Email/Library.h"
#include "Platform/SMTP/SMTP.h";
#include "Platform/POP3/POP3.h"
 
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
	bool stop = false;
	std::string inp;
	SentMail mail;
	do {


		TextPrinter::Print("Please choose Menu: \n");
		TextPrinter::Print("1. Send Mail\n");
		TextPrinter::Print("2. List Received Email\n");
		TextPrinter::Print("3. Exit\n");


		char choice = 0;
		TextPrinter::Print("Choose: ");
		std::cin >> choice;


		switch (choice)
		{
		case '1':
		{
			// Connect SMTP server
			m_Socket = Socket::Create({ AF_INET, SOCK_STREAM, IPPROTO_TCP });
			m_Socket->Connect("127.0.0.1", 2500);
			m_Socket->Receive();

			TextPrinter::Print("This is information mail: (If you don't have information, press ENTER to continue)\n");
			std::cin.ignore();

			TextPrinter::Print("From: ");
			inp = "";
			std::getline(std::cin, inp);
			if (!inp.empty())
				mail.Sender = inp;
			else TextPrinter::Print("<enter>\n");

			TextPrinter::Print("To: ");
			inp = "";
			std::getline(std::cin, inp);
			if (!inp.empty())
				mail.AddTo(inp);
			else TextPrinter::Print("<enter>\n");

			TextPrinter::Print("CC: ");
			inp = "";
			std::getline(std::cin, inp);
			if (!inp.empty())
				mail.AddCc(inp);
			else TextPrinter::Print("<enter>\n");

			TextPrinter::Print("Bcc: ");
			inp = "";
			std::getline(std::cin, inp);
			if (!inp.empty())
				mail.AddBcc(inp);
			else TextPrinter::Print("<enter>\n");

			TextPrinter::Print("Subject: ");
			inp = "";
			std::getline(std::cin, inp);
			if (!inp.empty())
				mail.Subject = inp;
			else TextPrinter::Print("<enter>\n");

			TextPrinter::Print("Content: ");
			inp = "";
			std::getline(std::cin, inp);
			if (!inp.empty())
				mail.Content.push_back(inp);
			else TextPrinter::Print("<enter>\n");

			int choice;
			TextPrinter::Print("Have attached files (1. YES, 2. NO)");
			std::cin >> choice;
			if (choice == 1) {
				TextPrinter::Print("How many file you want to send: ");

				int file_amount = 0;
				std::cin >> file_amount;
				std::cin.ignore();
				for (int i = 1; i <= file_amount; i++) {
					std::string path;
					TextPrinter::Print("Input directory " + std::to_string(i) + ": ");
					std::getline(std::cin, path);
					mail.AttachedFilePaths.push_back(path);
				}
			}
			bool isSent = SMTP::SendMail(m_Socket, mail);
			if (isSent) {
				TextPrinter::Print("Sending Success!\n",TextColor::Green);
			}
			else TextPrinter::Print("Sending Failed!\n", TextColor::Red);
			break;
		}

		case '2':
		{
			// Connect POP3 server
			m_Socket = Socket::Create({ AF_INET, SOCK_STREAM, IPPROTO_TCP });
			m_Socket->Connect("127.0.0.1", 1100);
			m_Socket->Receive();

			TextPrinter::Print("You are choosing: 2\n");
			TextPrinter::Print("This is list of folder in your mailbox: \n");
			TextPrinter::Print("1. Inbox");
			TextPrinter::Print("2. Project");
			TextPrinter::Print("3. Important");
			TextPrinter::Print("4. Work");
			TextPrinter::Print("5. Spam");
			int choice = 1;

			bool Two_Stop = false;
			do {
				
				TextPrinter::Print("This is list of mail in Inbox folder");
				Library mailContainer;

				POP3::LoginServer(m_Socket, mail.Sender, "112333");
				POP3::RetreiveMailSFromServer(m_Socket, mailContainer, "E:/Networking_project");
				for (int i = 0; i < mailContainer.GetRetrievedMails().size(); i++) {
					TextPrinter::Print(std::to_string(i + 1) + " <" + mailContainer.GetRetrievedMails()[i].Sender + "> <" +
						mailContainer.GetRetrievedMails()[i].Subject + ">\n");
				}
				
				choice = -1;
				TextPrinter::Print("Which mail you want to read: ");
				TextPrinter::Print("Or press ENTER to exit, or press 0 to see list email again");
				std::cin >> choice;
				if (choice == 0)
					continue;
				else if (choice == -1) {
					Two_Stop = true;
				}
				else {
					TextPrinter::Print("Content of " + std::to_string(choice) + ": \n");
					TextPrinter::Print(mailContainer.GetRetrievedMails()[choice - 1].Content);
				}
				if (mailContainer.GetRetrievedMails()[choice - 1].AttachedFiles.size() != 0) {
					int choice;
					TextPrinter::Print("This mail has attached files, do you want to save? (1:YES, 2:NO): ");
					std::cin >> choice;
					if (choice == 1) {
						std::string SavingPath;
						TextPrinter::Print("Input saving path: ");
						std::cin.ignore();
						std::getline(std::cin, SavingPath);

						for (int i = 0; i < mailContainer.GetRetrievedMails()[choice - 1].AttachedFiles.size(); i++) {
							std::string file_name = mailContainer.GetRetrievedMails()[choice - 1].AttachedFiles[i].FileName;
							mailContainer.GetRetrievedMails()[choice - 1].SaveFile(file_name, SavingPath);
						}
						
					}
				}
			} while (!Two_Stop);

		}

		case '3':
		{
			stop = true;
			break;
		}
		default:
			break;
		}
	} while (!stop);














	
	


	TextPrinter::Print("Sender: ");
	std::getline(std::cin, inp);
	mail.Sender = inp;

	TextPrinter::Print("To: ");
	std::getline(std::cin, inp);
	mail.AddTo(inp);
	
	


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
