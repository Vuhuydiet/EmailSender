#pragma once

#include <Client.h>

#include "Menu.h"

class UILayer : public Layer {
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnUIRender() override;
private:
	std::string GetUserInput(const std::string& notify = "", TextColor color = TextColor::Blue) const;
	template <typename Fn, typename ... Args>
	std::string GetUserInput(const std::string& notify, TextColor color, Fn condition, Args ... args) const;
	std::string GetUserInput(const std::string& notify, const std::set<std::string>& valid_selections, TextColor color = TextColor::Blue) const;

	void SaveConfigFiles() const;

	bool TestSMTPConnection() const;
	bool TestPOP3Connection() const;
private:
	Ref<Socket> m_Socket;

	Ref<Library> m_MailContainer = nullptr;
	Ref<MailFilter> m_MailFilter = nullptr;
private:
	Ref<Menu> m_ConnectToServer, m_Start, m_Login, m_MainMenu, m_SendMail, m_End, m_ShowFolders, m_AddKeyword, m_CreateFolder, m_ShowMails, m_DisplayMail, m_InputSavingFilePath, m_MoveMail;
	Ref<Menu> m_CurrentMenu;
};
