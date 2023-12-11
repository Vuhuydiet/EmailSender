#include "pch.h"
#include "SMTP.h"
#include "Base64/Base64.h"

#define _mB 1000000
static const int MAX_FILE_SIZE = 3*_mB; // size of file limit by 3MB

namespace SMTP {
    std::streampos getFileSize(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            __ERROR("Failed to open the file");
            return -1;
        }
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        return file.tellg();
    }

    void SendFile(Ref<Socket> socket, const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            __ERROR("Failed to open the file");
            return;
        }

        if (getFileSize(filePath) > MAX_FILE_SIZE) {
            __ERROR("FILE TOO LARGE");
            return;
        }

        std::string file_name = filePath.filename().string();
        std::string content_type = "Content-Type:application/octet-stream;charset=UTF-8;name=\"" + file_name +"\"";
        std::string content_dispostion = "Content-Disposition: attachment;filename=\"" + file_name + "\"";
        std::string content_trafer = "Content-Transfer-Encoding: base64";
        std::string space = "";

        socket->Send(content_type);
        socket->Send(content_dispostion);
        socket->Send(content_trafer);
        socket->Send(space);


        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            __ERROR("Error opening file");
            return;
        }
        //get content from file
        std::string file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // send content file
        std::string base64_encoded = base64::Base64_Encode(file_data);
        int line_length = 72; // divide data 
        for (size_t i = 0; i < base64_encoded.size(); i += line_length) {
            socket->Send(base64_encoded.substr(i, line_length));
        }
    }
}
