#include <pch.h>

#include "Base64.h"

namespace base64 {
    std::string Base64_Decode(const std::string& encoded_string) {
        const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        std::vector<int> decoding_table(256, -1);
        for (int i = 0; i < 64; ++i) {
            decoding_table[base64_chars[i]] = i;
        }

        std::vector<unsigned char> decoded_string;
        int val = 0, bits = -8;
        for (unsigned char c : encoded_string) {
            if (decoding_table[c] == -1) {
                __ERROR("Invalid character\n");
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