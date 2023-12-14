#pragma once

#include <Client.h>

class UILayer : public Layer {
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnUIRender() override;
private:
	SentMail MenuSendMail();
	void ListMail();

	std::string GetUserInput(const std::string& notify, std::function<bool(const std::string&)> condition = [](const std::string& inp) -> bool { return true; }, TextColor color = TextColor::Green) const;
	std::string GetUserInput(const std::string& notify, const std::set<std::string>& valid_selections, TextColor color = TextColor::Green) const;
private:
	Ref<Socket> m_Socket;

	Library m_MailContainer;

};
