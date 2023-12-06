#include "app_pch.h"
#include "Line.h"

#include <Renderer/TextPrinter.h>

void Line::Display() const 
{
	TextColor header_col = TextColor::White;
	TextColor msg_col = TextColor::White;
	switch (type)
	{
	case Line::Type::Server:
		header_col = TextColor::Green;
		msg_col = TextColor::Green;
		break;
	case Line::Type::Client:
		header_col = TextColor::Blue;
		msg_col = TextColor::White;
		break;
	case Line::Type::App:
		header_col = TextColor::Green;
		msg_col = TextColor::Green;
		break;
	case Line::Type::Info:
		header_col = TextColor::Blue;
		msg_col = TextColor::Blue;
		break;
	case Line::Type::Warn:
		header_col = TextColor::Yellow;
		msg_col = TextColor::Yellow;
		break;
	case Line::Type::Err:
		header_col = TextColor::Red;
		msg_col = TextColor::Red;
		break;
	}

	if (type != Line::Type::None)
		TextPrinter::Print(header + ": ", header_col);
	TextPrinter::Print(msg + '\n', msg_col);
}