#include <pch.h>

#include "Base64.h"

namespace base64 {

    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string Encode(const std::string& input) {
        std::string encoded;
        size_t i = 0;

        while (i < input.size()) {
            uint32_t octet_a = static_cast<uint8_t>(input[i++]);
            uint32_t octet_b = (i < input.size()) ? static_cast<uint8_t>(input[i++]) : 0;
            uint32_t octet_c = (i < input.size()) ? static_cast<uint8_t>(input[i++]) : 0;

            uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

            encoded.push_back(base64_chars[(triple >> 18) & 0x3F]);
            encoded.push_back(base64_chars[(triple >> 12) & 0x3F]);
            encoded.push_back((i < input.size() + 2) ? base64_chars[(triple >> 6) & 0x3F] : '=');
            encoded.push_back((i < input.size() + 1) ? base64_chars[triple & 0x3F] : '=');
        }
        return encoded;
    }

    std::string Decode(const std::string& encoded_string) {
        std::vector<int> decoding_table(256, -1);
        for (int i = 0; i < 64; ++i) {
            decoding_table[base64_chars[i]] = i;
        }

        std::vector<unsigned char> decoded_string;
        int val = 0, bits = -8;
        for (unsigned char c : encoded_string) {
            if (decoding_table[c] == -1) {
                __ERROR("Invalid character encountered when decoding base64_string\n");
                break; // Invalid character encountered
            }
            val = (val << 6) + decoding_table[c];
            bits += 6;

            if (bits >= 0) {
                decoded_string.push_back((val >> bits) & 0xFF);
                bits -= 8;
            }
        }

        return std::string(decoded_string.begin(), decoded_string.end());
    }

}