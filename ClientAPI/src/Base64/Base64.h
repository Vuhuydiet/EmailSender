#pragma once

#include <string>

namespace base64 {
	std::string Base64_Decode(const std::string& encoded_string);
	std::string Base64_Encode(const std::string& input);
}