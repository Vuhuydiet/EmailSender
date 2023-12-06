#include "pch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
	const char* pattern = "%^[%T %d-%m-%Y] [%l] %n: %v%$";

	auto filesink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("app.log");

	s_CoreLogger = spdlog::stdout_color_mt("LIB");
	s_CoreLogger->sinks()[0] = filesink;
	s_CoreLogger->set_pattern(pattern);
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger = spdlog::stdout_color_mt("APP");
	s_ClientLogger->sinks()[0] = filesink;
	s_ClientLogger->set_pattern(pattern);
	s_ClientLogger->set_level(spdlog::level::trace);
}
