#pragma once

#define SPDLOG_EOL ""
#include "spdlog/spdlog.h"

enum class TextColor { White, Blue, Green, Yellow, Red, RedWhite };

class TextPrinter {
public:
	template <typename ... Args>
	static void Print(const std::string& fmt, TextColor col = TextColor::White, Args ... args);

private:
	static void Init();
	friend class Application;
private:
	static std::shared_ptr<spdlog::logger> s_Printer;
};

template <typename ... Args>
inline void TextPrinter::Print(const std::string& text, TextColor col, Args ... args)
{
	switch (col)
	{
	case TextColor::White:
		s_Printer->trace(text, args...);
		break;
	case TextColor::Blue:
		s_Printer->debug(text, args...);
		break;
	case TextColor::Green:
		s_Printer->info(text, args...);
		break;
	case TextColor::Yellow:
		s_Printer->warn(text, args...);
		break;
	case TextColor::Red:
		s_Printer->error(text, args...);
		break;
	case TextColor::RedWhite:
		s_Printer->critical(text, args...);
		break;
	}
}