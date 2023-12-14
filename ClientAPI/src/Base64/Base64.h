#pragma once

#include <string>

namespace base64 {

	std::string Encode(const std::string& input);
	std::string Decode(const std::string& encoded_string);

}