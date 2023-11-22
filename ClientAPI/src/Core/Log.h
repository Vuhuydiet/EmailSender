#pragma once

#define SPDLOG_EOL ""
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <memory>

class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	static void Trace(std::shared_ptr<spdlog::logger>& logger, const std::string& msg) { logger->trace(msg + '\n'); }
	static void Info(std::shared_ptr<spdlog::logger>& logger, const std::string& msg) { logger->info(msg + '\n'); }
	static void Warn(std::shared_ptr<spdlog::logger>& logger, const std::string& msg) { logger->warn(msg + '\n'); }
	static void Error(std::shared_ptr<spdlog::logger>& logger, const std::string& msg) { logger->error(msg + '\n'); }
	static void Critical(std::shared_ptr<spdlog::logger>& logger, const std::string& msg) { logger->critical(msg + '\n'); }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

#if defined(_DEBUG)
	#if defined(_LIB)
		#define _TRACE(...)			::Log::Trace(::Log::GetCoreLogger(), __VA_ARGS__)
		#define _INFO(...)			::Log::Info(::Log::GetCoreLogger(), __VA_ARGS__)
		#define _WARN(...)			::Log::Warn(::Log::GetCoreLogger(), __VA_ARGS__)
		#define _ERROR(...)			::Log::Error(::Log::GetCoreLogger(), __VA_ARGS__)
		#define _CRITICAL(...)		::Log::Critical(::Log::GetCoreLogger(), __VA_ARGS__)
	#elif defined(_APP)
		#define _TRACE(...)			::Log::Trace(::Log::GetClientLogger(), __VA_ARGS__)
		#define _INFO(...)			::Log::Info(::Log::GetClientLogger(), __VA_ARGS__)
		#define _WARN(...)			::Log::Warn(::Log::GetClientLogger(), __VA_ARGS__)
		#define _ERROR(...)			::Log::Error(::Log::GetClientLogger(), __VA_ARGS__)
		#define _CRITICAL(...)		::Log::Critical(::Log::GetClientLogger(), __VA_ARGS__)
	#endif
#else
	#if defined(_LIB)
		#define _TRACE(...)		
		#define _INFO(...)		
		#define _WARN(...)		
		#define _ERROR(...)	
		#define _CRITICAL(...) 

	#elif defined(_APP)
		#define _TRACE(...)			
		#define _INFO(...)			
		#define _WARN(...)			
		#define _ERROR(...)
		#define _CRITICAL(...)
	#endif
#endif