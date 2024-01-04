#pragma once

#include <string>
#include <type_traits>

namespace std {
	
	inline string to_string(const std::string& arg) {
		return arg;
	}

	inline string to_string(const char* arg) {
		return arg;
	}
}

namespace FMT {

#define _RPC		"{}"
#define _RPC_LEN	strlen(_RPC)

#ifdef _OLD_CPP_FORMAT
	template <typename T>
	inline std::string format(const std::string& fmt, const T& arg) {
		std::string res = fmt;
		res.replace(res.find(_RPC), _RPC_LEN, std::to_string(arg));
		return res;
	}

	template <typename T, typename V>
	inline std::string format(const std::string& fmt, const T& arg1, const V& arg2) {
		std::string res = fmt;
		res.replace(res.find(_RPC), _RPC_LEN, std::to_string(arg1));
		res.replace(res.find(_RPC), _RPC_LEN, std::to_string(arg2));
		return res;
	}

	template <typename T, typename V, typename U>
	inline std::string format(const std::string& fmt, const T& arg1, const V& arg2, const U& arg3) {
		std::string res = fmt;
		res.replace(res.find(_RPC), _RPC_LEN, std::to_string(arg1));
		res.replace(res.find(_RPC), _RPC_LEN, std::to_string(arg2));
		res.replace(res.find(_RPC), _RPC_LEN, std::to_string(arg3));
		return res;
	}

#else
	template <typename... Args>
	inline std::string format_helper(const std::string& fmt, size_t start_pos) { return fmt; }

	template <typename T, typename ... Args> 
	inline std::string format_helper(const std::string& fmt, size_t start_pos, const T& arg1, Args ... args) {
		std::string res = fmt;
		start_pos = res.find(_RPC, (size_t)start_pos);
		res.replace(start_pos, _RPC_LEN, std::to_string(arg1));
		return format_helper(res, start_pos, args...);
	}

	template <typename ... Args>
	inline std::string format(const std::string& fmt, Args ... args) {
		return format_helper(fmt, 0, args...);
	};

#endif

	inline std::string cformat(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char buffer[1024];
		vsprintf_s(buffer, fmt, args);
		va_end(args);
		return std::string(buffer);
	}
}
