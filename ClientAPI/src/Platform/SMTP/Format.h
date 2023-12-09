#pragma once

#include <string>

namespace FMT {

#define _RPC		"{}"
#define _RPC_LEN	strlen(_RPC)

	inline std::string format(const std::string& fmt, const std::string& arg) {
		std::string res = fmt;
		res.replace(res.find(_RPC), _RPC_LEN, arg);
		return res;
	}

	inline std::string format(const std::string& fmt, const std::string& arg1, const std::string& arg2) {
		std::string res = fmt;
		res.replace(res.find(_RPC), _RPC_LEN, arg1);
		res.replace(res.find(_RPC), _RPC_LEN, arg2);
		return res;
	}

	inline std::string format(const std::string& fmt, const std::string& arg1, const std::string& arg2, const std::string& arg3) {
		std::string res = fmt;
		res.replace(res.find(_RPC), _RPC_LEN, arg1);
		res.replace(res.find(_RPC), _RPC_LEN, arg2);
		res.replace(res.find(_RPC), _RPC_LEN, arg2);
		return res;
	}

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
