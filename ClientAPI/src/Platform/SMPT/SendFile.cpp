#include "pch.h"
#include "SMTP.h"

static const int maxSizeFile = 3145728; // size of file limit by 3MB

static std::streampos getFileSize(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cout<<"Error opening file: " << filePath << std::endl;
        return -1;
    }

    return file.tellg();
}

bool checkSizeOfFile(std::string path) {
    int size = getFileSize(path);

    if (size == -1) return 0;

    if (size > maxSizeFile) {
        return 0;
    }
    return 1;
}

static std::string base64_encode(const std::string& input) {
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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

void SendFile(Ref<Socket> socket, const std::string& file_path) {
   /* if (!checkSizeOfFile(file_path)) {
        __WARN("FILE TOO LARGE!");
        return;
    }*/

    std::string file_name;
    for (int i = file_path.length() - 1; file_path[i] != '/'; i--) {
        file_name = file_path[i] + file_name;
    }

    static std::string content_type = "Content-Type: application/octet-stream; charset=UTF-8; name=" + file_name;
    static std::string content_dispostion = "Content-Disposition: attachment; filename=" + file_name;
    static std::string content_trafer = "Content-Transfer-Encoding: base64";
    static std::string space = "\n";
        
    socket->Send(content_type);
    socket->Send(content_dispostion);
    socket->Send(content_trafer);
    socket->Send(space);

        
    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    //get content from file
    std::string file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
    // send content file
    std::string base64_encoded = base64_encode(file_data);
    
    int line_length = 72; // divide data 
    for (size_t i = 0; i < base64_encoded.size(); i += line_length) {
        socket->Send(base64_encoded.substr(i, line_length));
    }
}
