#include "app_pch.h"
#include "UILayer.h"
 
#include "Config/Config.h"

#include "DefaultVars/Vars.h"
#include "HelperFunctions.h"

#define _YES 1
#define _NO  0

void UILayer::OnAttach() 
{
	CreateDirsIfNotExist({ _DEFAULT_HOST_MAILBOX_DIR, _DEFAULT_CONFIG_DIR });

	auto& config = Config::Get();
	config.Init(_DEFAULT_APP_CONFIG_FILEPATH);
	config.Load();

	m_Start					= CreateRef<Menu>("Start", false);
	m_Login					= CreateRef<Menu>("Login", false);
	m_Menu					= CreateRef<Menu>("Menu", false);
	m_SendMail				= CreateRef<Menu>("SendMail", false);
	m_End					= CreateRef<Menu>("End", false);
	m_ShowFolders			= CreateRef<Menu>("ShowFolder", false);
	m_ShowMails				= CreateRef<Menu>("ShowMails", false);
	m_DisplayMail			= CreateRef<Menu>("DisplayMails", false);
	m_MoveMail				= CreateRef<Menu>("MoveMail", false);
	m_InputSavingFilePath	= CreateRef<Menu>("InputSavingPath", false);

	m_CurrentMenu = m_Start;

	static std::string s_shown_folder;
	static Ref<RetrievedMail> s_shown_mail = nullptr;

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

		std::string choice = GetUserInput("Your choice: ", { "1", "2" }, Blue);
		if (choice == "1")
			m_Start->next = m_Login; 
		else if (choice == "2")
			m_Start->next = m_End;

	});

	m_Login->SetFunction([&]() {
		const std::string valid_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@.";
		if (!config.IsLoggedIn()) {
			TextPrinter::Print("Login to your account\n", Green);
			std::string username = GetUserInput("Account: ", Green, ContainsOnly, valid_chars);
			std::string password = GetUserInput("Password: ", Green, ContainsOnly, valid_chars);
			config.LogIn(username, password);
		}
		TextPrinter::Print("Succesfully logged in to '{}'!\n", Yellow, config.Username());

		const std::string& username = config.Username();
		const std::filesystem::path user_mailbox_dir = _DEFAULT_HOST_MAILBOX_DIR / username;
		CreateDirIfNotExists(user_mailbox_dir);

		m_MailContainer = CreateRef<Library>(user_mailbox_dir);
		m_MailContainer->CreateFolder("Inbox");
		m_MailContainer->CreateFolder("Project");
		m_MailContainer->CreateFolder("Important");
		m_MailContainer->CreateFolder("Work");
		m_MailContainer->CreateFolder("Spam");

		m_MailContainer->SetDefaultFolder("Inbox");

		auto& mailbox_config = m_MailContainer->GetMailboxConfig();
		mailbox_config.Load(user_mailbox_dir / "MailboxConfig.yml");

		m_MailContainer->LoadMails();

		m_Login->next = m_Menu;
	});

	m_Menu->SetFunction([&]() {
		const std::string menu =
			"Choose action:\n"
			"1. Send mail\n"
			"2. Show Folders\n"
			"3. Log out\n"
			"4. Exit\n";
		TextPrinter::Print(menu, Green);
		std::string choice = GetUserInput("Your choice: ", { "1", "2", "3", "4" }, Blue);
		if (choice == "1")
			m_Menu->next = m_SendMail;
		else if (choice == "2")
			m_Menu->next = m_ShowFolders;
		else if (choice == "3") {
			m_MailContainer->GetMailboxConfig().Save(_DEFAULT_HOST_MAILBOX_DIR / config.Username() / "MailboxConfig.yml");
			m_MailContainer = nullptr;
			config.LogOut();
			m_Menu->next = m_Start;
		}
		else if (choice == "4")
			m_Menu->next = m_End;
	});

	m_SendMail->SetFunction([&]() {
		TextPrinter::Print("These are your email's information.\n");

		SentMail mail;

		mail.Sender = config.Username();

		std::string tos = GetUserInput("To: ",Blue, [&](const std::string& input)->bool {
			return !input.empty();
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

		mail.Content.push_back(GetUserInput("Content: "));

		std::string have_file = GetUserInput("Have attached files (1. YES, 0. NO): ", { "1", "0" });
		if (have_file == "1") {
			std::string file_amount = GetUserInput("Number of file(s) you want to send (1 to 5): ", Green, IsNumberFromTo, 1, 5);
			for (int i = 1; i <= std::stoi(file_amount); i++) {
				std::string path = GetUserInput(FMT::format("Input directory {}: ", i), Blue, [&](const std::string& path) -> bool {
					if (!std::filesystem::exists(path)) {
						TextPrinter::Print("Input file does not exist!\n", Red);
						return false;
					}
					if (std::filesystem::file_size(path) > config.MaxSentFileSize()) {
						TextPrinter::Print("Input file is too large. File size limit is {} MB.\n", Red, config.MaxSentFileSize() >> 20);
						return false;
					}
					return true;
				});
				mail.AttachedFilePaths.push_back(path);
			}
		}

		std::string sent = GetUserInput("Do you want to send (y/n)?: ", { "y", "n" }, Yellow);
		if (sent == "y") {
			// Use socket to send mail
			m_Socket = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
			m_Socket->Connect(config.MailServer(), config.SMTP_Port());
			if (!m_Socket->IsConnected()) {
				TextPrinter::Print("Can not connect to the server, unable to send mail!", TextColor::Red);
				return;
			}
			TextPrinter::Print("\nSending your email...\n");
			bool isSent = SMTP::SendMail(m_Socket, mail);
			isSent ? TextPrinter::Print("Sending Success!\n\n", TextColor::Green) : TextPrinter::Print("Sending Failed!\n\n", TextColor::Red);
			SMTP::EndSession(m_Socket);
				m_Socket->Disconnect();
				m_Socket = nullptr;
		}

		m_SendMail->next = m_Menu;
	});

	m_End->SetFunction([&]() {
		if (config.IsLoggedIn()) {
			m_MailContainer->GetMailboxConfig().Save(_DEFAULT_HOST_MAILBOX_DIR / config.Username() / "MailboxConfig.yml");
			m_MailContainer = nullptr;
		}
		config.Save();
		Application::Get().Close();
		TextPrinter::Print("Closing application...", Green);
	});
   
	m_ShowFolders->SetFunction([&]() {
		TextPrinter::Print("Folders in your account: \n");
		m_MailContainer->LoadMails();
		const auto& folders = m_MailContainer->GetRetrievedMails();
		const auto& no_sorted_folders = m_MailContainer->GetAddedFolder();
		int i = 1;
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
>>>>>>> Stashed changes
		std::vector<std::string> folder_names;
		for (const auto& folder_mails : no_sorted_folders) {
			TextPrinter::Print("{}. {}\n", Blue, i, folder_mails);
			folder_names.push_back(folder_mails);
<<<<<<< Updated upstream
=======
=======
		for (const auto& folder_name : no_sorted_folders) {
			TextPrinter::Print("{}. {} ({})\n", Blue, i, folder_name, folders.at(folder_name).size());
>>>>>>> Stashed changes
>>>>>>> Stashed changes
			i++;
		}
		TextPrinter::Print("\n'm': return to Menu \n'Enter' : reload folder \nChoose folder:\n", Blue);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), Blue, [&](const std::string& inp) -> bool {
			return inp == "m" || inp.empty() || IsNumberFromTo(inp, 1, i - 1);
		});
		
		if (choice == "m") {
			m_ShowFolders->next = m_Menu;
		}
		else if (choice.empty()) {
			m_ShowFolders->next = m_ShowFolders;
		}
		else {
			int ind = std::stoi(choice) - 1;
			s_shown_folder = std::string(folder_names[ind]);
			m_ShowFolders->next = m_ShowMails;
		}
	});
	
	m_ShowMails->SetFunction([&]() {
		m_MailContainer->LoadMails();
		auto& mails = m_MailContainer->GetRetrievedMails(s_shown_folder);
		auto& mailbox_config = m_MailContainer->GetMailboxConfig();

		TextPrinter::Print("These are your retrieved mails in '{}':\n", Green, s_shown_folder);
		if (mails.empty())
			TextPrinter::Print("(empty)\n", White);

		int i = 1;
		for (auto& mail : mails) {
			bool read = mailbox_config.GetReadStatus(mail->Id);
			TextPrinter::Print("{}.{} [{}] - {}\n", Yellow, i, (read ? "" : " (not seen) "), mail->Sender, mail->Subject);
			i++;
		}
		
		std::string choice = "m";
		if (!mails.empty()) {
			TextPrinter::Print("\n'm': return to Menu \n'd': show folders \nChoose mail:\n", Blue);
			choice = GetUserInput(FMT::format("{} >> ", config.Username()), Blue, [&](const std::string& inp) {
				return inp == "m" || inp == "d" || IsNumberFromTo(inp, 1, i - 1);
			});
		}
		else {
			TextPrinter::Print("\n'm': return to Menu \n'd': show folders\n", Blue);
			choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "d" }, Blue);
		}

		if (choice == "m") {
			m_ShowMails->next = m_Menu;
		}
		else if (choice == "d") {
			m_ShowMails->next = m_ShowFolders;
		}
		else {
			s_shown_mail = mails[std::stoi(choice) - 1];
<<<<<<< Updated upstream

			choice = GetUserInput("Enter \"move\" to move mail and \"display\" to display mail: ", { "move", "display" }, Blue);
			if (choice == "display") {
<<<<<<< Updated upstream
=======
=======
			TextPrinter::Print("\n'dp': display mail \n'mv': move mail\n", Blue);
			choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "mv", "dp" }, Blue);
			if (choice == "dp") {
>>>>>>> Stashed changes
>>>>>>> Stashed changes
				m_ShowMails->next = m_DisplayMail;
			}
			else {
				m_ShowMails->next = m_MoveMail;
			}
		}
	});

	m_DisplayMail->SetFunction([&]() {
		auto& mailbox_config = m_MailContainer->GetMailboxConfig();
		mailbox_config.SetReadStatus(s_shown_mail->Id, true);

		TextPrinter::Print("The is the content of your mail.\n\n");
		TextPrinter::Print("{}\n\n", White, s_shown_mail->ToString());

		if (!s_shown_mail->AttachedFiles.empty() && 
			GetUserInput("Do you want to download the attached files? (y/n):", { "y", "n" }, Blue) == "y") 
		{
			m_DisplayMail->next = m_InputSavingFilePath;
			return;
		}

		TextPrinter::Print("'r': reply \n'm': return to Menu \n'mails': return to Mail List\n", Blue);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), {"r", "m", "mails" }, Blue);
		if (choice == "m")
			m_DisplayMail->next = m_Menu;
		else if (choice == "r")
			m_DisplayMail->next = m_SendMail;
		else
			m_DisplayMail->next = m_ShowMails;
	});

	m_MoveMail->SetFunction([&]() {
		std::string destination_folder;
		destination_folder = GetUserInput("Enter destination folder: ", Blue, [&](const std::string& dir) {
			return m_MailContainer->HasFolder(dir);
		});
		TextPrinter::Print("Moving mail...\n", Green);

		if (m_MailContainer->MoveMail(s_shown_folder, s_shown_mail->Id, destination_folder)) {
			TextPrinter::Print("Successful moving!\n", Yellow);
		}
		else {
			TextPrinter::Print("Can't move mail!\n");
		}
		TextPrinter::Print("\n'r': reply \n'm': return to Menu \n'mails': return to Mail List\n", Blue);
		std::string choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "d", "mails" }, Blue);
		if (choice == "m")
			m_MoveMail->next = m_Menu;
		else if (choice == "d")
			m_MoveMail->next = m_ShowFolders;
		else
			m_MoveMail->next = m_ShowMails;
	});

	m_InputSavingFilePath->SetFunction([&]() {
		std::string choice = GetUserInput("Input download folder path: ", Blue, [](const std::string& inp) {
			return std::filesystem::is_directory(inp);
		});

		TextPrinter::Print("\nChoose mail to download \n'a': download all\n", Blue);
		std::string downloaded_mail = GetUserInput(FMT::format("{} >> ", config.Username()), Blue);

		if (downloaded_mail == "a") {
			for (auto& file : s_shown_mail->AttachedFiles) {
				s_shown_mail->SaveFile(file.FileName, choice);
			}
			TextPrinter::Print("Downloading files...\n", Green);
			TextPrinter::Print("Download successfully!\n", Yellow);
		}
		else {
			const auto& selected_folder = m_MailContainer->GetRetrievedMails(s_shown_folder);
			if (std::stoi(downloaded_mail) >= 1 && std::stoi(downloaded_mail) <= selected_folder.size()) {
				s_shown_mail->SaveFile(s_shown_mail->AttachedFiles[std::stoi(downloaded_mail)-1].FileName, choice);
				TextPrinter::Print("Downloading mail...\n", Green);
				TextPrinter::Print("Successful download!\n", Yellow);
			}
			else {
				TextPrinter::Print("Wrong order number of mail!");
			}
		}
		TextPrinter::Print("\n'm': return to Menu \n'mails': return to Mail List\n", Blue);
		choice = GetUserInput(FMT::format("{} >> ", config.Username()), { "m", "mails" }, Blue);
		if (choice == "m")
			m_InputSavingFilePath->next = m_Menu;
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