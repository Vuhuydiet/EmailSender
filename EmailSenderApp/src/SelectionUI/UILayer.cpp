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

			char have_file = 0;
			do {
				TextPrinter::Print("Have attached files (1. YES, 2. NO): ");
				std::cin >> have_file;
				if (have_file == '1') {
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
				else if (have_file != '2') {
					TextPrinter::Print("Wrong Syntax! Please input again!\n", TextColor::Red);
					continue;
				}
				bool isSent = SMTP::SendMail(m_Socket, mail);
				if (isSent) {
					TextPrinter::Print("Sending Success!\n", TextColor::Green);
				}
				else TextPrinter::Print("Sending Failed!\n", TextColor::Red);
			} while (have_file != '1' && have_file != '2');
			break;
		}

		case '2':
		{
			TextPrinter::Print("You are choosing: 2\n");

			// Connect POP3 server
			m_Socket = Socket::Create({ AF_INET, SOCK_STREAM, IPPROTO_TCP });
			m_Socket->Connect("127.0.0.1", 1100);
			m_Socket->Receive();
			TextPrinter::Print("Login to server\n");
			TextPrinter::Print("Email: ");
			std::cin.ignore();
			std::string email;
			std::getline(std::cin, email);
			TextPrinter::Print("Password: ");
			std::string password;
			std::getline(std::cin, password);
			POP3::LoginServer(m_Socket, email, password);

			char chosen_folder_order = 0;
			do {
				TextPrinter::Print("This is list of folder in your mailbox: \n");
				TextPrinter::Print("1. Inbox\n");
				TextPrinter::Print("2. Project\n");
				TextPrinter::Print("3. Important\n");
				TextPrinter::Print("4. Work\n");
				TextPrinter::Print("5. Spam\n");
				TextPrinter::Print("Choose folder: ");
				std::cin >> chosen_folder_order;
				if (chosen_folder_order < '1' || chosen_folder_order > '5') {
					TextPrinter::Print("Wrong Syntax! Please input again!\n", TextColor::Red);
				}
			} while (chosen_folder_order < '1' || chosen_folder_order > '5');

			bool stop_case_2 = false;
			std::string chosen_folder;
			switch (chosen_folder_order) {
			case '1':
				chosen_folder = "Inbox";
				break;
			case '2':
				chosen_folder = "Project";
				break;
			case '3':
				chosen_folder = "Important";
				break;
			case '4':
				chosen_folder = "Work";
				break;
			case '5':
				chosen_folder = "Spam";
				break;
			}

			Library mailContainer;

			// TEMP
			// using temporary path
			POP3::RetreiveMailSFromServer(m_Socket, mailContainer, "MSG");

			do {
				// TEMP
				TextPrinter::Print("This is list of mail in " + chosen_folder + " folder:\n\n");

				
				for (int i = 0; i < mailContainer.GetRetrievedMails().size(); i++) {
					TextPrinter::Print(std::to_string(i + 1) + " <" + mailContainer.GetRetrievedMails()[i].Sender + "> <" +
						mailContainer.GetRetrievedMails()[i].Subject + ">\n");
				}
				
				int mail_order = -1;
				TextPrinter::Print("\nWhich mail you want to read");
				TextPrinter::Print("(Or press ENTER to exit, or press 0 to see list email again): ");
				std::cin >> mail_order;
				if (mail_order == 0)
					continue;
				else if (mail_order == -1) {
					stop_case_2 = true;
				}
				else {
					TextPrinter::Print("Content of " + std::to_string(mail_order) + ": \n");
					TextPrinter::Print(mailContainer.GetRetrievedMails()[mail_order - 1].Content + "\n");

					if (mailContainer.GetRetrievedMails()[mail_order - 1].AttachedFiles.size() != 0) {
						char save_file;
						TextPrinter::Print("This mail has attached files, do you want to save? (1:YES, 2:NO): ");
						std::cin >> save_file;
						if (save_file == '1') {
							std::string SavingPath;
							TextPrinter::Print("Input saving path: ");
							std::cin.ignore();
							std::getline(std::cin, SavingPath);

							for (int i = 0; i < mailContainer.GetRetrievedMails()[mail_order - 1].AttachedFiles.size(); i++) {
								std::string file_name = mailContainer.GetRetrievedMails()[mail_order - 1].AttachedFiles[i].FileName;
								mailContainer.GetRetrievedMails()[mail_order - 1].SaveFile(file_name, SavingPath);
							}

						}
					}
				}
			} while (!stop_case_2);

		}

		case '3':
		{
			m_Socket = nullptr;
			Application::Get().Close();
			TextPrinter::Print("Closing server...\n");
			stop = true;
			break;
		}
		default:
			break;
		}
	} while (!stop);


	


	/*TextPrinter::Print("Sender: ");
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
		Application::Get().Close();*/
}

void UILayer::OnUIRender()
{
}
