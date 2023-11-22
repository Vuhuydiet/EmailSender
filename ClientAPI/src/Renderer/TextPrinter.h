#pragma once

#define SPDLOG_EOL ""
#include "spdlog/spdlog.h"

enum class TextColor { White, Blue, Green, Yellow, Red, RedWhite };

class TextPrinter {
public:
	static void Print(const std::string& text, TextColor col = TextColor::White);

private:
	static void Init();
	friend class Application;
private:
	static std::shared_ptr<spdlog::logger> s_Printer;
};