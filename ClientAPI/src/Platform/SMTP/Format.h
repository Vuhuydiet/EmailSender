#pragma once

#include <string>

namespace std {
	
	string to_string(const std::string& arg) {
		return arg;
	}

	string to_string(const char* arg) {
		return arg;
	}
}

namespace FMT {

#define _RPC		"{}"
#define _RPC_LEN	strlen(_RPC)

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
}
