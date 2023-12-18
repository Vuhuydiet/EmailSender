#include "pch.h"
#include "SMTP.h"
#include "Base64/Base64.h"

namespace SMTP {

    void SendFile(Ref<Socket> socket, const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            __ERROR("Failed to open the file");
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
        std::string base64_encoded = base64::Encode(file_data);
        socket->Send(base64_encoded);
    }
}
