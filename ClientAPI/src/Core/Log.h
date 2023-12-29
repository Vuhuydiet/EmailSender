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

std::string Add_NL_Helper(const char* str);
std::string Add_NL_Helper(const std::string& str);

//#if defined(_DEBUG)
	#if defined(_LIB)
		#define __TRACE(x, ...)			::Log::GetCoreLogger()->trace(Add_NL_Helper(x), __VA_ARGS__)
		#define __DEBUG(x, ...)			::Log::GetCoreLogger()->debug(Add_NL_Helper(x), __VA_ARGS__)
		#define __INFO(x, ...)			::Log::GetCoreLogger()->info(Add_NL_Helper(x), __VA_ARGS__)
		#define __WARN(x, ...)			::Log::GetCoreLogger()->warn(Add_NL_Helper(x), __VA_ARGS__)
		#define __ERROR(x, ...)			::Log::GetCoreLogger()->error(Add_NL_Helper(x), __VA_ARGS__)
		#define __CRITICAL(x, ...)		::Log::GetCoreLogger()->critical(Add_NL_Helper(x), __VA_ARGS__)
	#elif defined(_APP)
		#define __TRACE(x, ...)			::Log::GetClientLogger()->trace(Add_NL_Helper(x), __VA_ARGS__)
		#define __DEBUG(x, ...)			::Log::GetClientLogger()->debug(Add_NL_Helper(x), __VA_ARGS__)
		#define __INFO(x, ...)			::Log::GetClientLogger()->info(Add_NL_Helper(x), __VA_ARGS__)	
		#define __WARN(x, ...)			::Log::GetClientLogger()->warn(Add_NL_Helper(x), __VA_ARGS__)	
		#define __ERROR(x, ...)			::Log::GetClientLogger()->error(Add_NL_Helper(x), __VA_ARGS__)	
		#define __CRITICAL(x, ...)		::Log::GetClientLogger()->critical(Add_NL_Helper(x), __VA_ARGS__)	
	#endif
//#else
//	#if defined(_LIB)
//		#define __TRACE(...)	
//		#define __DEBUG(...)
//		#define __INFO(...)		
//		#define __WARN(...)		
//		#define __ERROR(...)	
//		#define __CRITICAL(...) 
//
//	#elif defined(_APP)
//		#define __TRACE(...)	
//		#define __DEBUG(...)
//		#define __INFO(...)			
//		#define __WARN(...)			
//		#define __ERROR(...)
//		#define __CRITICAL(...)
//	#endif
//#endif