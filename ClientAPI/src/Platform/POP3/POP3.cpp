#include "pch.h"
#include "POP3.h"
#include "Core/Format.h"


namespace POP3 {

    void LoginServer(Ref<Socket>mail_receiver, const std::string& user_name, const std::string& password) {
        std::string userCommand = "USER " + user_name;
        std::string buffer = "";
        mail_receiver->Send(userCommand);
        buffer = mail_receiver->Receive();

        std::string passCommand = "PASS " + password;
        mail_receiver->Send(passCommand);
        buffer = mail_receiver->Receive();
    }

    // Find boundary in received mail
    static std::vector<std::string> FindMailIDs(Ref<Socket> mail_receiver, size_t mail_amount) {
        mail_receiver->Send("UIDL");
        std::string buffer = mail_receiver->Receive();
        std::vector<std::string> mail_ids;
        // find the mail want to find id
        int id_line = 0;
        std::string id;
        std::string ignored_line;
        std::stringstream buffer_stream(buffer);
        while(ignored_line.find("OK") == std::string::npos) 
                std::getline(buffer_stream, ignored_line);
        for (int i = 0; i < mail_amount; i++) {
            buffer_stream >> id_line;
            buffer_stream >> id;
            mail_ids.push_back(id);
        }

        return mail_ids;
    }

    static void PreprocessRawMail(std::string& buffer) {
        // erase '\r'
        buffer.erase(std::remove_if(buffer.begin(), buffer.end(), [](char c) -> bool { return c == '\r'; }), buffer.end());
        // erase fisrt line (server response "OK")
        buffer.erase(buffer.begin(), buffer.begin() + buffer.find_first_of('\n') + 1);
        // erase last line ('.')
        const std::string lastline = ".\n";
        buffer.erase(buffer.size() - lastline.size());
    }

    static std::vector<size_t> GetSizeOfMails(Ref<Socket> socket) {
        std::vector<size_t> sizeOfMails;
        socket->Send("LIST");
        std::stringstream ss(socket->Receive());
        std::string ignored_line;
        std::getline(ss, ignored_line);
        std::string line;
        while (std::getline(ss, line)) {
            line.pop_back();
            if (line == ".")
                break;
            std::stringstream ls(line);
            int sz = 0;
            ls >> sz >> sz;
            sizeOfMails.push_back(sz);
        }
        return sizeOfMails;
    }

    void RetrieveMailsFromServer(Ref<Socket>mail_receiver, const std::filesystem::path& mailbox_folder_path) {
        CreateDirIfNotExists(mailbox_folder_path);

        std::set<std::string> downloaded_mails;
        for (const auto& item : std::filesystem::directory_iterator(mailbox_folder_path)) {
            if (item.path().extension() != ".msg")
                continue;
            const std::filesystem::path path = item.path();
            downloaded_mails.insert(path.filename().string());
        }
        std::vector<size_t> sizeOfMails = GetSizeOfMails(mail_receiver);
        size_t mail_amount = sizeOfMails.size();
        std::vector<std::string> mail_ids = FindMailIDs(mail_receiver, mail_amount);
        for (int i = 0; i < mail_amount; i++) {
            std::string id = mail_ids[i];
            if (_found(downloaded_mails, id))
                continue;
            
            mail_receiver->Send(FMT::format("RETR {}", i + 1));
            std::string buffer = mail_receiver->Receive("\r\n.\r\n");

            PreprocessRawMail(buffer);
            std::ofstream mail_file(mailbox_folder_path / id);
            mail_file.write(buffer.c_str(), buffer.length());
            mail_file.close();
        }
    }
}