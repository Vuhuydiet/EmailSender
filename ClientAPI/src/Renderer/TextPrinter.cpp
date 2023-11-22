#include "pch.h"
#include "TextPrinter.h"

#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> TextPrinter::s_Printer;

void TextPrinter::Init()
{
	s_Printer = spdlog::stdout_color_mt("Text Printer");
	s_Printer->set_level(spdlog::level::trace);

	s_Printer->set_pattern("%^%v%$");
}

void TextPrinter::Print(const std::string& text, TextColor col) 
{
	switch (col)
	{
	case TextColor::White:
		s_Printer->trace(text);
		break;
	case TextColor::Blue:
		s_Printer->debug(text);
		break;
	case TextColor::Green:
		s_Printer->info(text);
		break;
	case TextColor::Yellow:
		s_Printer->warn(text);
		break;
	case TextColor::Red:
		s_Printer->error(text);
		break;
	case TextColor::RedWhite:
		s_Printer->critical(text);
		break;
	}
}