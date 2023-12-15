#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <memory>

#include <Client.h>

class Menu {
public:
	Menu(const std::string& name)
		: m_Name(name)
	{
	}

	void SetFunction(const std::function<void()>& fn) { m_RunFn = fn; }
	const std::string& GetName() const { return m_Name; }

	void Run() {
		m_RunFn(); 
	}

public:
	Ref<Menu> next = nullptr;
private:
	std::string m_Name;
	std::function<void()> m_RunFn = []() {};
};