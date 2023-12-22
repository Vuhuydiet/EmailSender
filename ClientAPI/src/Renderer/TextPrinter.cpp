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

void TextPrinter::Shutdown()
{
	s_Printer->flush();
	s_Printer.reset();
}

