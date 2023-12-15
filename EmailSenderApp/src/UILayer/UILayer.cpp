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

	m_Start = CreateRef<Menu>("Start");
	m_Login = CreateRef<Menu>("Login");
	m_Menu = CreateRef<Menu>("Menu");
	m_SendMail = CreateRef<Menu>("SendMail");
	m_End = CreateRef<Menu>("End");
	m_ShowFolders = CreateRef<Menu>("ShowFolder");
	m_ShowMails = CreateRef<Menu>("ShowMails");
	m_DisplayMail = CreateRef<Menu>("DisplayMails");
	m_InputSavingFilePath = CreateRef<Menu>("InputSavingPath");

	m_CurrentMenu = m_Start;

	static std::string s_showed_folder;
	static Ref<RetrievedMail> s_showed_mail = nullptr;

	m_Start->SetFunction([&]() { 
		m_Start->next = m_Login; 
	});

	m_Login->SetFunction([&]() {
		const std::string valid_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@.";
		if (!config.IsLoggedIn()) {
			TextPrinter::Print("Login to your account\n", Green);
			std::string username = GetUserInput("Account: ", Green, ContainsOnly, valid_chars);
			std::string password = GetUserInput("Password: ", Green, ContainsOnly, valid_chars);
			config.LogIn(username, password);
		}

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
		std::string choice = GetUserInput("Your choice: ", { "1", "2", "3", "4" }, Green);
		if (choice == "1")
			m_Menu->next = m_SendMail;
		else if (choice == "2")
			m_Menu->next = m_ShowFolders;
		else if (choice == "3") {
			m_MailContainer->GetMailboxConfig().Save(_DEFAULT_HOST_MAILBOX_DIR / config.Username() / "MailboxConfig.yml");
			m_MailContainer = nullptr;
			config.LogOut();
			m_Menu->next = m_Login;
		}
		else if (choice == "4")
			m_Menu->next = m_End;
	});

	m_SendMail->SetFunction([&]() {
		TextPrinter::Print("These are your email's information.\n");

		SentMail mail;

		mail.Sender = GetUserInput("From: ");

		std::string tos = GetUserInput("To: ");
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
			std::string file_amount = GetUserInput("How many file you want to send: ", Green, IsNumber);
			for (int i = 1; i <= std::stoi(file_amount); i++) {
				std::string path = GetUserInput(FMT::format("Input directory {}: ", i));
				mail.AttachedFilePaths.push_back(path);
			}
		}

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

		m_SendMail->next = m_Menu;
	});

	m_End->SetFunction([&]() {
		m_MailContainer->GetMailboxConfig().Save(_DEFAULT_HOST_MAILBOX_DIR / config.Username() / "MailboxConfig.yml");
		m_MailContainer = nullptr;
		config.Save();
		Application::Get().Close();
		TextPrinter::Print("Closing application...", Green);
	});
   
	m_ShowFolders->SetFunction([&]() {
		TextPrinter::Print("Folders in your account: \n");
		const auto& folders = m_MailContainer->GetRetrievedMails();

		int i = 1;
		std::vector<std::string> folder_names;
		for (const auto& folder_mails : folders) {
			TextPrinter::Print("{}. {}\n", Blue, i, folder_mails.first);
			folder_names.push_back(folder_mails.first);
			i++;
		}
		std::string choice = GetUserInput("Select a folder ('m': return to Menu): ", Blue, [&](const std::string& inp) -> bool {
			return inp == "m" || IsNumberFromTo(inp, 1, i - 1);
		});
		
		if (choice == "m") {
			m_ShowFolders->next = m_Menu;
		}
		else {
			int ind = std::stoi(choice) - 1;
			std::string folder = std::string(folder_names[ind]);
			m_ShowFolders->next = m_ShowMails;
		}
	});

	m_ShowMails->SetFunction([&]() {
		auto& mails = m_MailContainer->GetRetrievedMails(s_showed_folder);
		auto& mailbox_config = m_MailContainer->GetMailboxConfig();

		TextPrinter::Print("These are your retrieved mails in '{}':\n", Green, s_showed_folder);
		int i = 1;
		for (auto& mail : mails) {
			bool read = mailbox_config.GetReadStatus(mail->Id);
			TextPrinter::Print("{}.{}<{}> <{}>\n", Yellow, i, (read ? "" : " <not read> "), mail->Sender, mail->Subject);
			i++;
		}

		std::string choice = GetUserInput("Select an email ('m' to Menu, 'd' to show folders): \n", Blue, [&](const std::string& inp) {
			return inp == "m" || inp == "d" || IsNumberFromTo(inp, 1, i - 1);
		});

		if (choice == "m") {
			m_ShowMails->next = m_Menu;
		}
		else if (choice == "d") {
			m_ShowMails->next = m_ShowFolders;
		}
		else {
			s_showed_mail = mails[std::stoi(choice) - 1];
			m_ShowMails->next = m_DisplayMail;
		}
	});

	m_DisplayMail->SetFunction([&]() {
		auto& mailbox_config = m_MailContainer->GetMailboxConfig();
		mailbox_config.SetReadStatus(s_showed_mail->Id, true);

		TextPrinter::Print("The is the content of your mail.\n\n");
		TextPrinter::Print("{}\n\n", White, s_showed_mail->ToString());

		if (!s_showed_mail->AttachedFiles.empty()) {
			std::string choice = GetUserInput("Do you want to download the attached files? (y/n):", { "y", "n" }, Blue);
			if (choice == "y") {
				m_DisplayMail->next = m_InputSavingFilePath;
				return;
			}
		}

		std::string choice = GetUserInput("'m' to return Menu or 'mails' to return to Mail List: ", { "m", "mails" }, Blue);
		if (choice == "m")
			m_DisplayMail->next = m_Menu;
		else
			m_DisplayMail->next = m_ShowMails;
	});

	m_InputSavingFilePath->SetFunction([&]() {
		std::string choice = GetUserInput("Input download folder path: ", Blue, [](const std::string& inp) {
			return std::filesystem::is_directory(inp);
		});
		for (auto& file : s_showed_mail->AttachedFiles) {
			s_showed_mail->SaveFile(file.FileName, choice);
		}

		choice = GetUserInput("'m' to return Menu or 'mails' to return to Mail List: ", { "m", "mails" }, Blue);
		if (choice == "m")
			m_DisplayMail->next = m_Menu;
		else
			m_DisplayMail->next = m_ShowMails;
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