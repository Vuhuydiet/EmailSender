#include "app_pch.h"
#include "UILayer.h"
 
#include "Config/Config.h"

#include "DefaultVars/Vars.h"
#include <Utils/Utils.h>

#define _YES 1
#define _NO  0

#define _MAIL_FILTER_CONFIG_YML			"MailFilterConfig.yml"
#define _MAILBOX_CONFIG_YML				"MailboxConfig.yml"

void UILayer::OnAttach() 
{
	auto& config = Config::Get();

	m_ConnectToServer		= CreateRef<Menu>("ConnectToServer", true);
	m_Start					= CreateRef<Menu>("Start", true);
	m_Login					= CreateRef<Menu>("Login", false);
	m_MainMenu				= CreateRef<Menu>("Menu", true);
	m_SendMail				= CreateRef<Menu>("SendMail", false);
	m_End					= CreateRef<Menu>("End", false);
	m_ShowFolders			= CreateRef<Menu>("ShowFolder", true);
	m_AddKeyword			= CreateRef<Menu>("AddKeyword", true);
	m_CreateFolder			= CreateRef<Menu>("CreateFolder", true);
	m_ShowMails				= CreateRef<Menu>("ShowMails", true);
	m_DisplayMail			= CreateRef<Menu>("DisplayMails", false);
	m_MoveMail				= CreateRef<Menu>("MoveMail", true);
	m_InputSavingFilePath	= CreateRef<Menu>("InputSavingPath", true);

	m_CurrentMenu = m_ConnectToServer;

	static std::string s_shown_folder;
	static Ref<RetrievedMail> s_shown_mail = nullptr;

	m_ConnectToServer->SetFunction([&]() {
		auto smtp = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
		auto pop3 = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
		smtp->Connect(config.MailServer(), config.SMTP_Port());
		pop3->Connect(config.MailServer(), config.POP3_Port());
		bool is_connected = smtp->IsConnected() && pop3->IsConnected();
		smtp->Disconnect();
		pop3->Disconnect();

		if (!is_connected) {
			TextPrinter::Print("Can not connect to the server, unable to send mail!\n", TextColor::Red);
			TextPrinter::Print("Enter to restart the application\n", TextColor::Yellow);
			GetUserInput();
			Application::Get().Restart();
			m_ConnectToServer->next = m_ConnectToServer;
			return;
		}
		m_ConnectToServer->next = m_Start;
	});

	m_Start->SetFunction([&]() {
		if (config.IsLoggedIn()) {
			m_Start->next = m_Login;
			return;
		}

		const std::string menu =
			"Welcome the EmailSender!\n"
			"Please Login to continue.\n"
			"1. Login\n"
			"2. Exit\n";
		TextPrinter::Print(menu, Green);

		std::string choice = GetUserInput(">> ", { "1", "2" }, Blue);
		if (choice == "1")
			m_Start->next = m_Login; 
		else if (choice == "2")
			m_Start->next = m_End;

	});

	m_Login->SetFunction([&]() {
		const std::string valid_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@.";
		if (!config.IsLoggedIn()) {
			TextPrinter::Print("Login to your account\n", Green);
			std::string username = GetUserInput("Account: ", Green, [&](const std::string& input) {
				return !input.empty() && ContainsOnly(input, valid_chars);
			});
			std::string password = GetUserInput("Password: ", Green, [&](const std::string& input) {
				return !input.empty() && ContainsOnly(input, valid_chars);
			});
			config.LogIn(username, password);
		}
		Menu::Clear();

		TextPrinter::Print("Succesfully logged in to '{}'!\n", Yellow, config.Username());

		const std::string& username = config.Username();
		const std::filesystem::path user_mailbox_dir = _DEFAULT_HOST_MAILBOX_DIR / username;
		CreateDirIfNotExists(user_mailbox_dir);

		m_MailFilter = CreateRef<MailFilter>();
		if (std::filesystem::exists(user_mailbox_dir / _MAIL_FILTER_CONFIG_YML))
			m_MailFilter->Load(user_mailbox_dir / _MAIL_FILTER_CONFIG_YML);
		else
			m_MailFilter->Load(_DEFAULT_MAIL_FILTER_CONFIG_PATH);

		m_MailContainer = CreateRef<Library>(user_mailbox_dir);
		if (std::filesystem::exists(user_mailbox_dir / _MAILBOX_CONFIG_YML)) {
			m_MailContainer->LoadMailboxConfig(user_mailbox_dir / _MAILBOX_CONFIG_YML);
		}
		else {
			m_MailContainer->CreateFolder("Inbox");
			m_MailContainer->CreateFolder("Project");
			m_MailContainer->CreateFolder("Important");
			m_MailContainer->CreateFolder("Work");
			m_MailContainer->CreateFolder("Spam");

			m_MailContainer->SetDefaultFolder("Inbox");
			m_MailContainer->LoadMails(*m_MailFilter);
		}

		m_Login->next = m_MainMenu;
	});

	m_MainMenu->SetFunction([&]() {
		const std::string menu =
			"Choose action:\n"
			"1. Send mail\n"
			"2. Show Folders\n"
			"3. Log out\n"
			"4. Exit\n";
		TextPrinter::Print(menu, Green);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "1", "2", "3", "4"}, Blue);
		if (choice == "1")
			m_MainMenu->next = m_SendMail;
		else if (choice == "2")
			m_MainMenu->next = m_ShowFolders;
		else if (choice == "3") {
			SaveConfigFiles();
			m_MailFilter = nullptr;
			m_MailContainer = nullptr;
			config.LogOut();
			m_MainMenu->next = m_Start;
		}
		else if (choice == "4")
			m_MainMenu->next = m_End;
	});

	m_SendMail->SetFunction([&]() {
		TextPrinter::Print("These are your email's information.\n");

		SentMail mail;

		mail.Sender = config.Username();

		std::string tos = GetUserInput("To: ", Blue, [&](const std::string& inp)->bool{
			return !inp.empty();
		});
		RemoveChar(tos, ' ');
		mail.Tos = Split(tos, ',');

		std::string ccs = GetUserInput("Cc: ");
		RemoveChar(ccs, ' ');
		mail.Ccs = Split(ccs, ',');

		std::string bccs = GetUserInput("Bcc: ");
		RemoveChar(bccs, ' ');
		mail.Bccs = Split(bccs, ',');

		mail.Subject = GetUserInput("Subject: ");

		TextPrinter::Print("Content (enter a line with '.' to end email content):\n", Blue);
		std::string line;
		while ((line = GetUserInput("")) != ".") {
			mail.Content.push_back(line);
		}

		std::string have_file = GetUserInput("Have attached files (y/n): ", { "y", "n" });
		if (have_file == "y") {
			std::string file_amount = GetUserInput("Number of file(s) you want to send: ", Green, IsNumber);
			for (int i = 1; i <= std::stoi(file_amount); i++) {
				std::string path = GetUserInput(FMT::format("Input directory {}: ", i), Blue, [&](const std::string& path) -> bool {
					if (!std::filesystem::is_regular_file(path)) {
						TextPrinter::Print("Input file does not exist!\n", Red);
						return false;
					}
					if (std::filesystem::file_size(path) > config.MaxSentFileSize()) {
						TextPrinter::Print("Input file is too large. File size limit is {}MB.\n", Red, config.MaxSentFileSize() >> 20);
						return false;
					}
					return true;
				});
				mail.AttachedFilePaths.push_back(path);
			}
		}
		std::string choice = GetUserInput("Do you want to send mail? (y/n): ", { "y", "n" }, Yellow);
		if (choice == "n") {
			Menu::Clear();
			m_SendMail->next = m_MainMenu;
			return;
		}
		// Use socket to send mail
		m_Socket = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
		m_Socket->Connect(config.MailServer(), config.SMTP_Port());
		if (!m_Socket->IsConnected()) {
			Menu::Clear();
			m_Socket->Disconnect();
			m_Socket = nullptr;
			m_SendMail->next = m_ConnectToServer;
			return;
		}
		TextPrinter::Print("\nSending your email...\n");
		bool isSent = SMTP::SendMail(m_Socket, mail);
		isSent ? TextPrinter::Print("Sending Success!\n\n", TextColor::Green) : TextPrinter::Print("Sending Failed!\n\n", TextColor::Red);

		SMTP::EndSession(m_Socket);

		m_Socket->Disconnect();
		m_Socket = nullptr;

		m_SendMail->next = m_MainMenu;
	});

	m_End->SetFunction([&]() {
		if (config.IsLoggedIn()) {
			SaveConfigFiles();
			m_MailFilter = nullptr;
			m_MailContainer = nullptr;
		}
		config.Save();
		Application::Get().Close();
		TextPrinter::Print("Closing application...", Green);
	});
   
	m_ShowFolders->SetFunction([&]() {
		TextPrinter::Print("Folders in your account: \n");
		m_MailContainer->LoadMails(*m_MailFilter);
		const auto& folders = m_MailContainer->GetRetrievedMails();
		const auto& no_sorted_folders = m_MailContainer->GetAddedFolder();
		int i = 1;
		for (const auto& folder_name : no_sorted_folders) {
			TextPrinter::Print("{}. {} ({})\n", Blue, i, folder_name, folders.at(folder_name).size());
			i++;
		}
		
		TextPrinter::Print("\n'm': return to Menu \n'Enter': reload folder \n'c': create folder \nSelect a folder :\n", Yellow);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), Blue, [&](const std::string& inp) -> bool {
			return inp == "m" || inp.empty() || inp == "c" || IsNumberFromTo(inp, 1, i - 1);
		});
		
		if (choice == "m") {
			Menu::Clear();
			m_ShowFolders->next = m_MainMenu;
		}
		else if (choice.empty()) {
			Menu::Clear();
			m_ShowFolders->next = m_ShowFolders;
		}
		else if (choice == "c") {
			Menu::Clear();
			m_ShowFolders->next = m_CreateFolder;
		}
		else {
			int ind = std::stoi(choice) - 1;
			s_shown_folder = no_sorted_folders[ind];
			m_ShowFolders->next = m_ShowMails;
		}
	});

	m_AddKeyword->SetFunction([&]() {
		
		TextPrinter::Print("Select filter type: \n1. From \n2. Subject \n3. Content\n", Yellow);
		std::string filter_type_choice = GetUserInput(FMT::format("{} >> ", config.Username()), Blue, IsNumberFromTo, 1, 3);
		FilterType filter_type;
		if (filter_type_choice == "1") {
			filter_type = FilterType::From;
		}
		else if (filter_type_choice == "2") {
			filter_type = FilterType::Subject;
		}
		else if (filter_type_choice == "3") {
			filter_type = FilterType::Content;
		}
		std::string keyword = GetUserInput("Input keyword: ", Blue);
		m_MailFilter->AddKeyword(keyword, s_shown_folder, filter_type);

		TextPrinter::Print("\n'm': return to Menu \n'mails': return to Mail List \n", Yellow);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "mails"}, Blue);
		if (choice == "m")
			m_AddKeyword->next = m_MainMenu;
		else
			m_AddKeyword->next = m_ShowMails;
		Menu::Clear();
	});

	m_CreateFolder->SetFunction([&]() {
		TextPrinter::Print("Input folder name:\n");
		std::string folder_name = GetUserInput(FMT::format("{} >> ", config.Username()), Blue, [&](const std::string& inp) -> bool {
			return !inp.empty();
			});

		m_MailContainer->CreateFolder(folder_name);
		m_CreateFolder->next = m_MainMenu;
	});
	
	m_ShowMails->SetFunction([&]() {
		m_MailContainer->LoadMails(*m_MailFilter);
		auto& mails = m_MailContainer->GetRetrievedMails(s_shown_folder);

		TextPrinter::Print("These are your retrieved mails in '{}':\n\n", Green, s_shown_folder);

		if (mails.empty())
			TextPrinter::Print("(empty)\n", White);

		int i = 1;
		for (auto& mail : mails) {
			bool read = m_MailContainer->GetReadStatus(mail->Id);
			TextPrinter::Print("{}. {}[{}] - {}\n", Yellow, i, (read ? "" : "(*) "),  mail->Sender, mail->Subject);
			i++;
		}
		
		std::string choice = "m";
		TextPrinter::Print("\n'm': return to Menu \n'k': add keyword \n'd': show folders\n", Yellow);
		choice = GetUserInput(FMT::format("{} >> ", config.Username()), Blue, [&](const std::string& inp) {
			return inp == "m" || inp == "d" || inp == "k" || IsNumberFromTo(inp, 1, i - 1);
		});

		m_ShowMails->SetAutoClear(true);
		if (choice == "m") {
			m_ShowMails->next = m_MainMenu;
		}
		else if (choice == "k") {
			m_ShowMails->next = m_AddKeyword;
		}
		else if (choice == "d") {
			m_ShowMails->next = m_ShowFolders;
		}
		else {
			s_shown_mail = mails[std::stoi(choice) - 1];

			TextPrinter::Print("'dp': display mail \n'mv': move mail \n", Yellow);
			choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "mv", "dp" }, Blue);
			if (choice == "dp") {
				m_ShowMails->next = m_DisplayMail;
			}
			else {
				m_ShowMails->SetAutoClear(false);
				m_ShowMails->next = m_MoveMail;
			}
		}
	});

	m_DisplayMail->SetFunction([&]() {
		m_MailContainer->SetReadStatus(s_shown_mail->Id, true);

		TextPrinter::Print("{}", White, s_shown_mail->ToString());

		if (s_shown_mail->AttachedFiles.empty()) {
			TextPrinter::Print("This email does not contain attached files.\n", Yellow);
		}
		else {
			TextPrinter::Print("Number of attached files: {}\n", Yellow, s_shown_mail->AttachedFiles.size());

		}

		for (int i = 0; i < s_shown_mail->AttachedFiles.size(); i++) {
			TextPrinter::Print("{}. {}\n", Blue, i + 1, s_shown_mail->AttachedFiles[i].FileName);
		}

		if (!s_shown_mail->AttachedFiles.empty() &&
			GetUserInput("\nDo you want to download the attached files? (y/n): ", { "y", "n" }, Yellow) == "y")
		{
			m_DisplayMail->next = m_InputSavingFilePath;
			return;
		}

		TextPrinter::Print("\n'm': return to Menu \n'mails': return to Mail List \n'r': reply\n", Yellow);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "mails", "r" }, Blue);
		if (choice == "m")
			m_DisplayMail->next = m_MainMenu;
		else if (choice == "r")
			m_DisplayMail->next = m_SendMail;
		else
			m_DisplayMail->next = m_ShowMails;
		Menu::Clear();
	});

	m_MoveMail->SetFunction([&]() {
		std::string destination_folder;
		destination_folder = GetUserInput("Enter destination folder: ", Blue, [&](const std::string& dir) {
			return m_MailContainer->HasFolder(dir);
		});

		Menu::Clear();

		if (m_MailContainer->MoveMail(s_shown_folder, s_shown_mail->Id, destination_folder))
			TextPrinter::Print("Successful moving!\n", Green);
		else
			TextPrinter::Print("Can't move mail!\n", Red);

		TextPrinter::Print("\n'm': return to Menu \n'd': return to folders \n'mails': return to Mail List\n", Yellow);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "d", "mails" }, Blue);
		if (choice == "m")
			m_MoveMail->next = m_MainMenu;
		else if (choice == "d")
			m_MoveMail->next = m_ShowFolders;
		else
			m_MoveMail->next = m_ShowMails;
	});

	m_InputSavingFilePath->SetFunction([&]() {
		std::string downloaded_file = GetUserInput("Enter the file ID you want to download or 'a' to download all attached files: ", Yellow, [&](const std::string& inp) {
			return inp == "a" || IsNumberFromTo(inp, 1, (int)s_shown_mail->AttachedFiles.size());
		});
		
		std::string dir = GetUserInput("Input download folder path: ", Blue, [](const std::string& inp) {
			return inp.empty() || std::filesystem::is_directory(inp);
		});
		if (dir.empty())
			dir = _DEFAULT_DOWNLOAD_DIR.string();

		TextPrinter::Print("Downloading files...\n", Green);
		if (downloaded_file == "a")
			s_shown_mail->SaveAllFiles(dir);
		else
			s_shown_mail->SaveFile(std::stoi(downloaded_file) - 1, dir);
		TextPrinter::Print("Downloaded successfully!\n", Yellow);
		
		TextPrinter::Print("'m': return Menu \n'mails': return to Mail List\n", Yellow);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "mails" }, Blue);
		if (choice == "m")
			m_InputSavingFilePath->next = m_MainMenu;
		else
			m_InputSavingFilePath->next = m_ShowMails;
	});
}

void UILayer::OnDetach()
{
}


void UILayer::OnUpdate(float dt) {
	m_CurrentMenu->Run();
	m_CurrentMenu = m_CurrentMenu->next;
}

void UILayer::OnUIRender()
{
}

std::string UILayer::GetUserInput(const std::string& notify, TextColor color) const {
	TextPrinter::Print(notify, color);
	std::string input;
	std::getline(std::cin, input);
	return input;
}

template <typename Fn, typename ... Args>
std::string UILayer::GetUserInput(const std::string& notify, TextColor color, Fn condition, Args ... args) const {
	TextPrinter::Print(notify, color);
	std::string input = "##input_error";
	bool good_selection = false;
	do {
		std::getline(std::cin, input);
		good_selection = condition(input, args...);
		if (!good_selection)
			TextPrinter::Print("Invalid selection! Please try again: ", TextColor::Yellow);
	} while (!good_selection);
	return input;
}

std::string UILayer::GetUserInput(const std::string& notify, const std::set<std::string>& valid_selections, TextColor color) const {
	return GetUserInput(notify, color, [&valid_selections](const std::string& input) -> bool {
		return valid_selections.empty() || _found(valid_selections, input); 
	});
}

void UILayer::SaveConfigFiles() const {
	const auto& config = Config::Get();
	std::filesystem::path user_mailbox_dir = _DEFAULT_HOST_MAILBOX_DIR / config.Username();
	m_MailFilter->Save(user_mailbox_dir / _MAIL_FILTER_CONFIG_YML);
	m_MailContainer->SaveMailboxConfig(user_mailbox_dir / _MAILBOX_CONFIG_YML);
}