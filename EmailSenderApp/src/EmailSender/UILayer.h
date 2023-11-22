#pragma once
#include <Client.h>

#include "ScreenInfo.h"

class UILayer : public Layer 
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnUIRender() override;

private:
	void ClearScreen() const;

private:
	ScreenInfo m_ScreenInfo;
};