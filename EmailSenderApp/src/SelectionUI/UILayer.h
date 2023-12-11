#pragma once

#include <Client.h>

class UILayer : public Layer {
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnUIRender() override;
private:
	void SendMail();
	void ListMail();
private:
	Ref<Socket> m_Socket;

};