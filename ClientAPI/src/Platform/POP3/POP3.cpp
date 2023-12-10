#include "pch.h"
#include "POP3.h"
#include "Platform/SMTP/Format.h"


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
    static std::vector<std::string> FindMailIDs(Ref<Socket> mail_receiver, int mail_amount) {
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

    static std::string FindBoundary(std::ifstream& fi) {
        

        if (!fi.is_open()) {
            __ERROR("Can't open file\n");
            return "";
        }

        std::string buffer;

        std::getline(fi, buffer);
        std::getline(fi, buffer);

        std::string boundaryString = "boundary=\"";

        std::string boundary = buffer.substr(buffer.find(boundaryString) + boundaryString.size());
        boundary.pop_back();

        return boundary;
    }

    static void SaveInfo(RetrievedMail& retreived_mail, const std::string& buffer) {
        std::stringstream ss(buffer);

        std::string boundary = FindBoundary(fi);
        std::string content = "";
        char delim = '\n';
        while (!fi.eof()) {
            std::getline(fi, buffer, delim);
            if (buffer.find("From: ") != std::string::npos) {
                buffer.pop_back();
                retreived_mail.Sender = buffer.substr(buffer.find(" ") + 1, buffer.length());
            }

            if (buffer.find("To: ") != std::string::npos) {
                buffer = buffer.substr(buffer.find(" ") + 1, buffer.length());
                while (!buffer.empty()) {
                    buffer.pop_back();
                    retreived_mail.Tos.push_back(buffer.substr(0, buffer.find(" ")));
                    buffer.erase(0, retreived_mail.Tos.back().size());
                }
            }

            if (buffer.find("Cc: ") != std::string::npos) {
                buffer = buffer.substr(buffer.find(" ") + 1);
                while (!buffer.empty()) {
                    buffer.pop_back();
                    retreived_mail.Ccs.push_back(buffer.substr(0, buffer.find(" ")));
                    buffer.erase(0, retreived_mail.Ccs.back().size());
                }
            }

            if (buffer.find("Subject: ") != std::string::npos) {
                buffer.pop_back();
                retreived_mail.Subject = buffer.substr(buffer.find(" ") + 1);
            }

            if (buffer.find("Content-Transfer-Encoding") != std::string::npos) {
                std::getline(fi, buffer, delim);
                std::getline(fi, buffer, delim);
                do {
                    buffer.pop_back();
                    content += buffer;
                    std::getline(fi, buffer, delim);
                } while (buffer.find(boundary) == std::string::npos);
                retreived_mail.Content = content;
            }

            if (buffer.find("filename") != std::string::npos) {
                content = "";
                FileInfo file_info;
                file_info.FileName = buffer.substr(buffer.find("\"") + 1);
                file_info.FileName.pop_back();
                file_info.FileName.pop_back();
                std::getline(fi, buffer, delim);
                std::getline(fi, buffer, delim);
                do {
                    std::getline(fi, buffer, delim);
                    if (buffer == "\r") break;
                    buffer.pop_back();
                    content += buffer;
                } while (1);
                file_info.Base64_Encoded_Content = content;
                retreived_mail.AttachedFiles.push_back(file_info);
            }
        }

        fi.close();
    }
    static bool IsR(char c)
    {
        return (c == '\r');
    }
    static void PreprocessRawMail(std::string& buffer) {
        buffer.erase(std::remove_if(buffer.begin(), buffer.end(), &IsR), buffer.end());
        buffer.erase(buffer.begin(), buffer.begin() + buffer.find_first_of('\n') + 1);
        buffer.pop_back();
    }

    std::vector<size_t> GetSizeOfMails(Ref<Socket> socket) {
        std::vector<size_t> res;

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
        return res;
    }

    void RetreiveMailSFromServer(Ref<Socket>mail_receiver, Library& mail_container, const std::filesystem::path& mailbox_folder_path) {
        std::vector<size_t> sizeOfMails = GetSizeOfMails(mail_receiver);
        int mail_amount = sizeOfMails.size();
        std::vector<std::string> mail_ids = FindMailIDs(mail_receiver, mail_amount);


        for (int i = 1; i <= mail_amount; i++) {
            std::string id = mail_ids[i-1];
            std::ofstream mail_file(mailbox_folder_path / id);
            
            mail_receiver->Send(FMT::format("RETR {}",i));
            std::string buffer = mail_receiver->Receive(sizeOfMails[i - 1]);

            PreprocessRawMail(buffer);
            mail_file.write(buffer.c_str(), buffer.length());
            mail_file.close();

            RetrievedMail retreived_mail;
            SaveInfo(retreived_mail, buffer);
            mail_container.AddNewMail(retreived_mail);
        }
    }
}