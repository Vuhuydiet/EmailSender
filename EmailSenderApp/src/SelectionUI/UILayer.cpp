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

void UILayer::SendMail() {
	std::string inp;
	SentMail mail;

	TextPrinter::Print("This is information mail: (If you don't have information, press ENTER to continue)\n",TextColor::Green);
	std::cin.ignore();

	TextPrinter::Print("From: ", TextColor::Green);
	inp = "";
	std::getline(std::cin, inp);
	if (!inp.empty())
		mail.Sender = inp;
	else TextPrinter::Print("<enter>\n", TextColor::White);

	TextPrinter::Print("To: ", TextColor::Green);
	inp = "";
	std::getline(std::cin, inp);
	if (!inp.empty())
		mail.AddTo(inp);
	else TextPrinter::Print("<enter>\n", TextColor::White);

	TextPrinter::Print("CC: ", TextColor::Green);
	inp = "";
	std::getline(std::cin, inp);
	if (!inp.empty())
	{
		std::stringstream ss(inp);
		std::string token;
		while (std::getline(ss, token, ' ')) {
			mail.AddCc(token);
		}

	}
	else TextPrinter::Print("<enter>\n", TextColor::White);

	TextPrinter::Print("Bcc: ", TextColor::Green);
	inp = "";
	std::getline(std::cin, inp);
	if (!inp.empty()) {
		std::stringstream ss(inp);
		std::string token;
		while (std::getline(ss, token, ' ')) {
			mail.AddBcc(inp);
		}
	}
	else TextPrinter::Print("<enter>\n", TextColor::White);

	TextPrinter::Print("Subject: ", TextColor::Green);
	inp = "";
	std::getline(std::cin, inp);
	if (!inp.empty())
		mail.Subject = inp;
	else TextPrinter::Print("<enter>\n", TextColor::White);

	TextPrinter::Print("Content: ", TextColor::Green);
	inp = "";
	std::getline(std::cin, inp);
	if (!inp.empty())
		mail.Content.push_back(inp);
	else TextPrinter::Print("<enter>\n", TextColor::White);

	char have_file = 0;
	do {
		TextPrinter::Print("Have attached files (1. YES, 2. NO): ", TextColor::Green);
		std::cin >> have_file;
		if (have_file == '1') {
			TextPrinter::Print("How many file you want to send: ", TextColor::Green);

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
	} while (have_file != '1' && have_file != '2');


	m_Socket->Init();
	m_Socket = Socket::Create({ AF_INET, SOCK_STREAM, IPPROTO_TCP });
	m_Socket->Connect("127.0.0.1", 2500);
	m_Socket->Receive();

	bool isSent = SMTP::SendMail(m_Socket, mail);
	if (isSent) {
		TextPrinter::Print("Sending Success!\n", TextColor::Green);
	}
	else TextPrinter::Print("Sending Failed!\n", TextColor::Red);

	m_Socket->Disconnect();
	m_Socket = nullptr;
}

void UILayer::ListMail() {

	TextPrinter::Print("You are choosing: 2\n");

	// Connect POP3 server
	m_Socket = Socket::Create({ AF_INET, SOCK_STREAM, IPPROTO_TCP });
	m_Socket->Connect("127.0.0.1", 1100);
	m_Socket->Receive();
	TextPrinter::Print("Login to server\n", TextColor::Green);
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
		TextPrinter::Print("This is list of folder in your mailbox: \n", TextColor::Green);
		TextPrinter::Print("1. Inbox\n", TextColor::Yellow);
		TextPrinter::Print("2. Project\n", TextColor::Yellow);
		TextPrinter::Print("3. Important\n", TextColor::Yellow);
		TextPrinter::Print("4. Work\n", TextColor::Yellow);
		TextPrinter::Print("5. Spam\n", TextColor::Yellow);
		TextPrinter::Print("Choose folder: ");
		std::cin >> chosen_folder_order;
		if (chosen_folder_order < '1' || chosen_folder_order > '5') {
			TextPrinter::Print("Wrong Syntax! Please input again!\n", TextColor::Red);
		}
	} while (chosen_folder_order < '1' || chosen_folder_order > '5');

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
	std::cin.ignore();

	do {
		// TEMP
		TextPrinter::Print("This is list of mail in " + chosen_folder + " folder:\n\n", TextColor::Green);


		for (int i = 0; i < mailContainer.GetRetrievedMails().size(); i++) {
			TextPrinter::Print(std::to_string(i + 1) + " <" + mailContainer.GetRetrievedMails()[i].Sender + "> <" +
				mailContainer.GetRetrievedMails()[i].Subject + ">\n", TextColor::Yellow);
		}

		std::string mail_order;
		TextPrinter::Print("\nWhich mail you want to read");
		TextPrinter::Print("(Or press ENTER to exit, or press 0 to see list email again): ");
		std::getline(std::cin, mail_order);
		if (mail_order == "0")
			continue;
		else if (mail_order.empty()) {
			break;
		}
		else {
			TextPrinter::Print("Content of " + mail_order + ": \n", TextColor::Green);
			TextPrinter::Print(mailContainer.GetRetrievedMails()[stoi(mail_order) - 1].ToString() + "\n", TextColor::Yellow);

			if (mailContainer.GetRetrievedMails()[stoi(mail_order) - 1].AttachedFiles.size() != 0) {
				char save_file;
				TextPrinter::Print("This mail has attached files, do you want to save? (1:YES, 2:NO): ");
				std::cin >> save_file;
				if (save_file == '1') {
					std::string SavingPath;
					TextPrinter::Print("Input saving path: ");
					std::cin.ignore();
					std::getline(std::cin, SavingPath);

					for (int i = 0; i < mailContainer.GetRetrievedMails()[stoi(mail_order) - 1].AttachedFiles.size(); i++) {
						std::string file_name = mailContainer.GetRetrievedMails()[stoi(mail_order) - 1].AttachedFiles[i].FileName;
						mailContainer.GetRetrievedMails()[stoi(mail_order) - 1].SaveFile(file_name, SavingPath);
					}

				}
			}
		}
	} while (true);
}

void UILayer::OnUpdate(float dt)
{
	
	std::string inp;
	SentMail mail;
	


	TextPrinter::Print("Please choose Menu: \n", TextColor::Blue);
	TextPrinter::Print("1. Send Mail\n", TextColor::Blue);
	TextPrinter::Print("2. List Received Email\n", TextColor::Blue);
	TextPrinter::Print("3. Exit\n", TextColor::Blue);


	char choice = 0;
	TextPrinter::Print("Choose: ");
	std::cin >> choice;

	switch (choice)
	{
	case '1':
	{
		SendMail();
		break;
	}

	case '2':
	{
		ListMail();
		break;
	}

	case '3':
	{
		m_Socket = nullptr;
		Application::Get().Close();
		TextPrinter::Print("Closing server...\n", TextColor::Green);
		break;
	}
	default:
		break;
	}

}

void UILayer::OnUIRender()
{
}
