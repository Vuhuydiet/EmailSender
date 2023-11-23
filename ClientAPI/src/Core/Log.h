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

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

inline static std::string Add_NL_Helper(const char* str) { return std::string(str) + '\n'; }
inline static std::string Add_NL_Helper(const std::string& str) { return str + '\n'; }

#if defined(_DEBUG)
	#if defined(_LIB)
		#define _TRACE(x, ...)			::Log::GetCoreLogger()->trace(Add_NL_Helper(x), __VA_ARGS__)
		#define _INFO(x, ...)			::Log::GetCoreLogger()->info(Add_NL_Helper(x), __VA_ARGS__)
		#define _WARN(x, ...)			::Log::GetCoreLogger()->warn(Add_NL_Helper(x), __VA_ARGS__)
		#define _ERROR(x, ...)			::Log::GetCoreLogger()->error(Add_NL_Helper(x), __VA_ARGS__)
		#define _CRITICAL(x, ...)		::Log::GetCoreLogger()->critical(Add_NL_Helper(x), __VA_ARGS__)
	#elif defined(_APP)
		#define _TRACE(x, ...)			::Log::GetClientLogger()->trace(Add_NL_Helper(x), __VA_ARGS__)	
		#define _INFO(x, ...)			::Log::GetClientLogger()->info(Add_NL_Helper(x), __VA_ARGS__)	
		#define _WARN(x, ...)			::Log::GetClientLogger()->warn(Add_NL_Helper(x), __VA_ARGS__)	
		#define _ERROR(x, ...)			::Log::GetClientLogger()->error(Add_NL_Helper(x), __VA_ARGS__)	
		#define _CRITICAL(x, ...)		::Log::GetClientLogger()->critical(Add_NL_Helper(x), __VA_ARGS__)	
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