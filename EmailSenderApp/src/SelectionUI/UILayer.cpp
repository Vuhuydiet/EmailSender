#include "app_pch.h"
#include "UILayer.h"
 
#include "DefaultVars/Vars.h"

#define _YES 1
#define _NO  0

void UILayer::OnAttach()
{
	if (!std::filesystem::is_directory(_DEFAULT_HOST_MAILBOX_DIR))
		std::filesystem::create_directory(_DEFAULT_HOST_MAILBOX_DIR);
}

void UILayer::OnDetach()
{
}

static bool IsNumber(const std::string& s) {
	return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


SentMail UILayer::MenuSendMail() {
	// Get email information
	TextPrinter::Print("This is the information of your email.\n");

	SentMail mail;

	mail.Sender = GetUserInput("From: ");

	std::string tos = GetUserInput("To: ");
	if (!tos.empty()) {
		std::stringstream ss(tos);
		std::string to;
		while (std::getline(ss, to, ' ')) {
			mail.AddTo(to);
		}
	}

	std::string ccs = GetUserInput("Cc: ");
	if (!ccs.empty()) {
		std::stringstream ss(ccs);
		std::string cc;
		while (std::getline(ss, cc, ' ')) {
			mail.AddCc(cc);
		}
	}

	std::string bccs = GetUserInput("Bcc: ");
	if (!bccs.empty()) {
		std::stringstream ss(bccs);
		std::string bcc;
		while (std::getline(ss, bcc, ' ')) {
			mail.AddBcc(bcc);
		}
	}

	mail.Subject = GetUserInput("Subject: ");

	mail.Content.push_back(GetUserInput("Content: "));

	std::string have_file = GetUserInput("Have attached files (1. YES, 0. NO): ", {"1", "0"});
	if (have_file == "0")
		return mail;
	
	std::string file_amount = GetUserInput("How many file you want to send: ", IsNumber);
	for (int i = 1; i <= std::stoi(file_amount); i++) {
		std::string path = GetUserInput(FMT::format("Input directory {}: ", i));
		mail.AttachedFilePaths.push_back(path);
	}

	return mail;
}

void UILayer::ListMail() {
	// Connect POP3 server
	//m_Socket = Socket::Create({ SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP });
	//m_Socket->Connect(_SERVER_DEFAULT_IP, _POP3_DEFAULT_PORT);
	//m_Socket->Receive();

	TextPrinter::Print("Login to server\n", TextColor::Green);
	std::string email = GetUserInput("Email: ");
	std::string password = GetUserInput("Password: ");
	
	// TEMP
	// TODO: write to config file
	//POP3::LoginServer(m_Socket, email, password);
	//POP3::RetreiveMailsFromServer(m_Socket, _DEFAULT_HOST_MAILBOX_DIR / email);

	if (!std::filesystem::is_directory(_DEFAULT_HOST_MAILBOX_DIR / email))
		std::filesystem::create_directory(_DEFAULT_HOST_MAILBOX_DIR / email);
	for (const auto& item : std::filesystem::directory_iterator(_DEFAULT_HOST_MAILBOX_DIR / email)) {
		const std::filesystem::path& path = item.path();
		m_MailContainer.AddNewMail(path);
	}

	const std::string menu =
		"This is list of folder in your mailbox: \n"
		"1. Inbox\n"
		"2. Project\n"
		"3. Important\n"
		"4. Work\n"
		"5. Spam\n";
	TextPrinter::Print(menu, TextColor::Green);
	std::string chosen_folder_order = GetUserInput("Choose folder: ", { "1", "2", "3", "4", "5" }, TextColor::Yellow);
	
	const std::map<std::string, std::string> choice_to_name = 
		{ {"1", "Inbox"}, {"2", "Project"}, {"3", "Important"}, {"4", "Work"}, {"5", "Spam"} };
	std::string chosen_folder = choice_to_name.at(chosen_folder_order);
	
	if (m_MailContainer.GetRetrievedMails().size() == 0) {
		TextPrinter::Print("There was no one texted you. Maybe find a friend in life.\n");
		return;
	}
	TextPrinter::Print("This is list of mail in {}\n\n", TextColor::White, chosen_folder);
	for (int i = 0; i < m_MailContainer.GetRetrievedMails().size(); i++) {
		const auto& mail = m_MailContainer.GetRetrievedMails()[i];
		TextPrinter::Print("{} <{}> <{}>\n", TextColor::White, i + 1, mail.Sender, mail.Subject);
	}

	std::function<bool(const std::string&)> is_valid_mail_order = [&](const std::string& input) -> bool {
		if (!IsNumber(input))
			return false;
		int choice = std::stoi(input);
		return choice >= 1 && choice <= m_MailContainer.Size();
	};

	std::string mail_order = GetUserInput("\nThe Email you want to read: ", is_valid_mail_order);
	int order = std::stoi(mail_order);
	const RetrievedMail& mail = m_MailContainer.GetRetrievedMails()[order - 1];
	TextPrinter::Print("Content of {}: \n{}\n", TextColor::Green, mail_order, mail.ToString());

	if (mail.AttachedFiles.empty())
		return;
	std::string save_file = GetUserInput("This mail has attached files, do you want to save? (1: YES, 0: NO): ", {"1", "0"});
	if (save_file == "0")
		return;

	std::string saving_path = GetUserInput("Input saving path: ");
	for (const auto& file_info : mail.AttachedFiles) {
		mail.SaveFile(file_info.FileName, saving_path);
	}
}

void UILayer::OnUpdate(float dt) {
	const std::string menu = 
		"Please choose Menu: \n"
		"1. Send Mail\n"
		"2. List Received Email\n"
		"3. Exit\n";
	TextPrinter::Print(menu, TextColor::Green);

	std::string choice = GetUserInput("Your choice: ", {"1", "2", "3"}, TextColor::Blue);
	do {
		if (choice == "1") {
			SentMail mail = MenuSendMail();

			m_Socket = Socket::Create({ SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP });
			m_Socket->Connect(_SERVER_DEFAULT_IP, _SMTP_DEFAULT_PORT);
			if (!m_Socket->IsConnected()) {
				TextPrinter::Print("Can not connect to the server, unable to send mail!", TextColor::Red);
				break;
			}
			TextPrinter::Print("\nSending your email...\n");
			bool isSent = SMTP::SendMail(m_Socket, mail);
			isSent ? TextPrinter::Print("Sending Success!\n\n", TextColor::Green) : TextPrinter::Print("Sending Failed!\n\n", TextColor::Red);

			m_Socket->Send("QUIT");
			m_Socket->Receive();

			m_Socket->Disconnect();
			m_Socket = nullptr;
		}
		else if (choice == "2") {
			ListMail();
		}
		else if (choice == "3") {
			m_Socket = nullptr;
			Application::Get().Close();
			TextPrinter::Print("Closing application...\n");
		}
	} while (false);
}

void UILayer::OnUIRender()
{
}

std::string UILayer::GetUserInput(const std::string& notify, std::function<bool(const std::string&)> condition, TextColor color) const {
	TextPrinter::Print(notify, color);
	std::string input = "##input_error";
	bool good_selection = false;
	do {
		std::getline(std::cin, input);
		good_selection = condition(input);
		if (!good_selection)
			TextPrinter::Print("Invalid selection! Please try again: ", TextColor::Yellow);
	} while (!good_selection);
	return input;
}

std::string UILayer::GetUserInput(const std::string& notify, const std::set<std::string>& valid_selections, TextColor color) const {
	return GetUserInput(notify, [&valid_selections](const std::string& input) -> bool {
		return valid_selections.empty() || _found(valid_selections, input); 
	}, color);
}