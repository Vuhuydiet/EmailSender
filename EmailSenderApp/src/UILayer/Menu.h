#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <memory>

#include <Client.h>

class Menu {
public:
	Menu(const std::string& name, bool autoclear = true)
		: m_Name(name), m_AutoClearScreen(autoclear)
	{
	}

	void SetFunction(const std::function<Ref<Menu>()>& fn) { m_RunFn = fn; }
	const std::string& GetName() const { return m_Name; }

	Ref<Menu> Run() {
		Ref<Menu> ret = m_RunFn(); 
		if (m_AutoClearScreen)
			system("CLS");
		return ret;
	}

	static void Clear() {
		system("CLS");
	}

	void SetAutoClear(bool clear) { m_AutoClearScreen = clear; }

private:
	std::string m_Name;
	std::function<Ref<Menu>()> m_RunFn = []() -> Ref<Menu> {return nullptr; };

	bool m_AutoClearScreen;
};