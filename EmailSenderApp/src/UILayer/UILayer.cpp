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
	m_ShowFolders			= CreateRef<Menu>("ShowFolder", false);
	m_AddKeyword			= CreateRef<Menu>("AddKeyword", false);
	m_CreateFolder			= CreateRef<Menu>("CreateFolder", true);
	m_ShowMails				= CreateRef<Menu>("ShowMails", true);
	m_DisplayMail			= CreateRef<Menu>("DisplayMails", false);
	m_MoveMail				= CreateRef<Menu>("MoveMail", true);
	m_InputSavingFilePath	= CreateRef<Menu>("InputSavingPath", true);

	m_CurrentMenu = m_Start;

	static std::string s_shown_folder;
	static Ref<RetrievedMail> s_shown_mail = nullptr;
	static std::string s_label;

	/*m_ConnectToServer->SetFunction([&]() {
		if (!TestSMTPConnection()) {
			TextPrinter::Print("Can not connect to the server, unable to send mail!\n", TextColor::Red);
			TextPrinter::Print("'Enter': restart the application\n'e': exit\n", TextColor::Yellow);
			std::string choice = GetUserInput(">>  ", Blue, [&](const std::string& inp) {
				return inp == "e" || inp.empty();
			});

			if (choice == "e") {
				return m_End;
			}
			if (choice.empty()) {
				Application::Get().Restart();
				return m_ConnectToServer;
			}
		}

		return m_Start;
	});*/

	// TEMP
	m_ConnectToServer = m_MainMenu;

	m_Start->SetFunction([&]() {
		if (config.IsLoggedIn())
			return m_Login;

		const std::string menu =
			"Welcome the EmailSender!\n"
			"Please Login to continue.\n"
			"1. Login\n"
			"2. Exit\n";
		TextPrinter::Print(menu, Green);

		std::string choice = GetUserInput(">> ", { "1", "2" }, Blue);
		if (choice == "1")
			return m_Login; 
		if (choice == "2")
			return m_End;
		return m_ConnectToServer;
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

		s_label = FMT::format("{} >> ", config.Username());

		const std::string& username = config.Username();
		const std::filesystem::path user_mailbox_dir = _DEFAULT_HOST_MAILBOX_DIR / username;
		CreateDirIfNotExists(user_mailbox_dir);

		m_MailFilter = CreateRef<MailFilter>();
		if (std::filesystem::exists(user_mailbox_dir / _MAIL_FILTER_CONFIG_YML))
			m_MailFilter->Load(user_mailbox_dir / _MAIL_FILTER_CONFIG_YML);
		else if (std::filesystem::exists(_DEFAULT_MAIL_FILTER_CONFIG_PATH))
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

		return m_MainMenu;
	});

	m_MainMenu->SetFunction([&]() {
		const std::string menu =
			"Choose action:\n"
			"1. Send mail\n"
			"2. Show Folders\n"
			"3. Log out\n"
			"4. Exit\n";
		TextPrinter::Print(menu, Green);
		std::string choice = GetUserInput(s_label, { "1", "2", "3", "4"}, Blue);
		if (choice == "1")
			return m_SendMail;
		if (choice == "2")
			return m_ShowFolders;
		if (choice == "3") {
			SaveConfigFiles();
			m_MailFilter = nullptr;
			m_MailContainer = nullptr;
			config.LogOut();
			return m_Start;
		}
		if (choice == "4")
			return m_End;

		return m_ConnectToServer;
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
						TextPrinter::Print("Input file is too large. File size limit is {}MB.\n", Red, config.MaxSentFileSize() * 1.0f / (1e6));
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
			return m_MainMenu;
		}

		m_Socket = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
		m_Socket->Connect(config.MailServer(), config.SMTP_Port());
		if (!m_Socket->IsConnected()) {
			Menu::Clear();
			TextPrinter::Print("Cannot connect to SMTP server, unable to send your email!\n", Red);
			m_Socket->Disconnect();
			m_Socket = nullptr;
			return m_ConnectToServer;
		}
		TextPrinter::Print("\nSending your email...\n");
		bool isSent = SMTP::SendMail(m_Socket, mail);
		isSent ? TextPrinter::Print("Sending Success!\n\n", TextColor::Green) : TextPrinter::Print("Sending Failed!\n\n", TextColor::Red);

		SMTP::EndSession(m_Socket);

		m_Socket->Disconnect();
		m_Socket = nullptr;

		return m_MainMenu;
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
		return nullptr;
	});
   
	m_ShowFolders->SetFunction([&]() {
		if (!TestPOP3Connection())
			TextPrinter::Print("Cannot connect to POP3 server, unable to receive mails!\n\n", Red);
		m_MailContainer->LoadMails(*m_MailFilter);

		const auto& folders = m_MailContainer->GetRetrievedMails();
		const auto& no_sorted_folders = m_MailContainer->GetAddedFolder();
		int i = 1;
		TextPrinter::Print("Folders in your account: \n");
		for (const auto& folder_name : no_sorted_folders) {
			TextPrinter::Print("{}. {} ({})\n", Blue, i, folder_name, folders.at(folder_name).size());
			i++;
		}
		
		TextPrinter::Print("\n'Enter': reload folders\n'c': create folder \n'm': return to Menu \n", Yellow);
		std::string choice = GetUserInput(s_label, Blue, [&](const std::string& inp) -> bool {
			return inp.empty() || inp == "m" || inp == "c" || IsNumberFromTo(inp, 1, (int)no_sorted_folders.size());
		});
		
		if (choice == "c")
			return m_CreateFolder;

		Menu::Clear();
		if (choice == "m")
			return m_MainMenu;
		if (choice.empty())
			return m_ShowFolders;
		
		int ind = std::stoi(choice) - 1;
		s_shown_folder = no_sorted_folders[ind];
		return m_ShowMails;
	});

	m_AddKeyword->SetFunction([&]() {
		TextPrinter::Print("Folder: '{}'\nSelect filter type: \n1. From \n2. Subject \n3. Content\n", Yellow, s_shown_folder);
		std::string filter_type_choice = GetUserInput(s_label, Blue, IsNumberFromTo, 1, 3);
		FilterType filter_type = (FilterType)std::stoi(filter_type_choice);
		std::string keyword = GetUserInput("Input keyword: ", Blue);
		m_MailFilter->AddKeyword(keyword, s_shown_folder, filter_type);

		Menu::Clear();
		TextPrinter::Print("Keyword '{}' has been added successfully!\n", Green, keyword);
	
		return m_ShowMails;
	});

	m_CreateFolder->SetFunction([&]() {
		std::string folder_name = GetUserInput("Input folder name: ", Blue, [&](const std::string& inp) -> bool {
			return !inp.empty();
		});

		m_MailContainer->CreateFolder(folder_name);
		return m_ShowFolders;
	});
	
	m_ShowMails->SetFunction([&]() {
		if (!TestPOP3Connection())
			TextPrinter::Print("Cannot connect to POP3 server, unable to receive mails!\n\n", Red);
		m_MailContainer->LoadMails(*m_MailFilter);

		std::vector<Ref<RetrievedMail>> mails = m_MailContainer->GetRetrievedMails(s_shown_folder);
		std::sort(mails.begin(), mails.end(), [](Ref<RetrievedMail> a, Ref<RetrievedMail> b) { return a->SendDate > b->SendDate; });

		TextPrinter::Print("These are your retrieved mails in '{}':\n\n", Green, s_shown_folder);

		if (mails.empty())
			TextPrinter::Print("(empty)\n", White);

		int i = 1;
		for (const auto& mail: mails) {
			bool read = m_MailContainer->GetReadStatus(mail->Id);
			std::string sendTime;
			if (mail->SendDate.SameDay(Date()))
				sendTime = FMT::format("{}:{}", mail->SendDate.hour, mail->SendDate.min);
			else
				sendTime = FMT::format("{}/{}/{}", mail->SendDate.day, mail->SendDate.month, mail->SendDate.year);
			TextPrinter::Print("{}. [{}] - {} ({}) {}\n", (read ? White : Yellow), i,  mail->Sender, mail->Subject, sendTime, (read ? "" : "(*)"));
			i++;
		}
		TextPrinter::Print("\n'Enter': reload mails\n'k': add keyword \n'd': show folders\n'm': return to Menu \n", Yellow);

		std::string choice = GetUserInput(s_label, Blue, [&](const std::string& inp) {
			return inp.empty() || inp == "m" || inp == "d" || inp == "k" || IsNumberFromTo(inp, 1, (int)mails.size());
		});

		m_ShowMails->SetAutoClear(true);
		if (choice.empty())
			return m_ShowMails;
		if (choice == "m")
			return m_MainMenu;
		if (choice == "k")
			return m_AddKeyword;
		if (choice == "d")
			return m_ShowFolders;
		
		int index = std::stoi(choice) - 1;
		s_shown_mail = mails[index];

		TextPrinter::Print("'dp': display mail \n'mv': move mail \n", Yellow);
		choice = GetUserInput(s_label, { "dp", "mv" }, Blue);
		if (choice == "dp")
			return m_DisplayMail;
		if (choice == "mv") {
			m_ShowMails->SetAutoClear(false);
			return m_MoveMail;
		}

		return m_ConnectToServer;
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
			return m_InputSavingFilePath;
		}

		TextPrinter::Print("\n'r': reply\n'mails': return to Mail List\n'm': return to Menu \n", Yellow);
		std::string choice = GetUserInput(s_label, { "m", "mails", "r" }, Blue);

		Menu::Clear();
		if (choice == "m")
			return m_MainMenu;
		if (choice == "r")
			return m_SendMail;
		if (choice == "mails")
			return m_ShowMails;

		return m_ConnectToServer;
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

		TextPrinter::Print("\n'mails': return to Mail List\n'd': return to folders \n'm': return to Menu \n", Yellow);
		std::string choice = GetUserInput(s_label, { "m", "d", "mails" }, Blue);
		if (choice == "m")
			return m_MainMenu;
		if (choice == "d")
			return m_ShowFolders;
		if (choice == "mails")
			return m_ShowMails;

		return m_ConnectToServer;
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

		TextPrinter::Print("\nDownloading file(s)...\n", Green);
		if (downloaded_file == "a")
			s_shown_mail->SaveAllFiles(dir);
		else
			s_shown_mail->SaveFile(std::stoi(downloaded_file) - 1, dir);
		TextPrinter::Print("Downloaded successfully!\n\n", Yellow);
		
		TextPrinter::Print("'mails': return to Mail List\n'm': return Menu \n", Yellow);
		std::string choice = GetUserInput(s_label, { "m", "mails" }, Blue);
		if (choice == "m")
			return m_MainMenu;
		if (choice == "mails")
			return m_ShowMails;

		return m_ConnectToServer;
	});
}

void UILayer::OnDetach()
{
}


void UILayer::OnUpdate(float dt) {
	Ref<Menu> next = m_CurrentMenu->Run();
	m_CurrentMenu = next;
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
	if (!m_MailContainer || !m_MailFilter)
		return;
	const auto& config = Config::Get();
	std::filesystem::path user_mailbox_dir = _DEFAULT_HOST_MAILBOX_DIR / config.Username();
	m_MailFilter->Save(user_mailbox_dir / _MAIL_FILTER_CONFIG_YML);
	m_MailContainer->SaveMailboxConfig(user_mailbox_dir / _MAILBOX_CONFIG_YML);
}

bool UILayer::TestSMTPConnection() const {
	const auto& config = Config::Get();
	auto smtp = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
	smtp->Connect(config.MailServer(), config.SMTP_Port());
	bool is_connected = smtp->IsConnected();
	smtp->Disconnect();
	smtp = nullptr;
	return is_connected;
}

bool UILayer::TestPOP3Connection() const {
	const auto& config = Config::Get();
	auto pop3 = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
	pop3->Connect(config.MailServer(), config.POP3_Port());
	bool is_connected = pop3->IsConnected();
	pop3->Disconnect();
	pop3 = nullptr;
	return is_connected;
}