#pragma once

#include "Log.h"

#define ASSERT(flag, msg) {\
	if (!flag) {\
		_CRITICAL("Assertion Failed!");\
		_CRITICAL(msg);\
		__debugbreak();\
	}\
}

#include <memory>

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}