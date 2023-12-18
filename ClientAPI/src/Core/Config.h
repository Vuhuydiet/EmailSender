#pragma once

#include "Log.h"

#define ASSERT(flag, msg, ...) {\
	if (!(flag)) {\
		__CRITICAL("Assertion Failed!");\
		__CRITICAL(msg, __VA_ARGS__);\
		__debugbreak();\
	}\
}

#include <memory>

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args... args) {
	return std::make_shared<T>(args...);
}

#define _found(ds, key) (ds.find(key) != ds.end())

#include <filesystem>
inline void CreateDirIfNotExists(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path))
		std::filesystem::create_directory(path);
}
inline void CreateDirsIfNotExist(const std::vector<std::filesystem::path>& paths) {
	for (const auto& path : paths) {
		if (!std::filesystem::exists(path))
			std::filesystem::create_directory(path);
	}
}