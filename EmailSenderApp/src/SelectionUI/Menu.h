#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <memory>

class Menu {
public:
	Menu() = default;
	Menu(const std::vector<std::pair<std::string, std::string>>& choice_input, const std::function<void()>& fn)
		: m_RunFn(fn), m_Choices(choice_input)
	{}
	void SetChoices(const std::vector<std::pair<std::string, std::string>>& choices) { m_Choices = choices; }
	void SetFunction(const std::function<void()>& fn) { m_RunFn = fn; }

	void Run() { m_RunFn(); }
	Menu* Next() const { return m_Next; }

public:
	Ref<Menu> next = nullptr;
private:
	std::function<void()> m_RunFn = []() {};
	std::vector<std::pair<std::string, std::string>> m_Choices;
};