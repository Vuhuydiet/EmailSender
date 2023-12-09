#include "pch.h"
#include "POP3.h"

namespace POP3 {

    void LoginServer(Ref<Socket>mail_receiver, const std::string& user_name, const std::string& password) {
        std::string userCommand = "USER " + user_name;
        std::string buffer = "";
        mail_receiver->Send(userCommand);
        mail_receiver->Receive();

        std::string passCommand = "PASS " + password;
        mail_receiver->Send(passCommand);
        mail_receiver->Receive();
    }

    static int CountMailAmount(Ref<Socket>mail_receiver) {
        int count = 0;
        char delim = '\r';
        mail_receiver->Send("LIST");
        std::string buffer = mail_receiver->Receive();

        for (auto charactor : buffer) {
            if (charactor == delim) count++;
        }

        return count + 1;
    }

    //***************************************************************************
    // Not used functions

    // File interacting functions

    // Helpers


    // Decode and save decoded file
    //static bool SaveDecodedFile(const std::string& encoded_string, const std::string& outputFilename) {
    //    std::string decoded_string = Base64_Decode(encoded_string);

    //    // Write the decoded data to a file
    //    std::ofstream outputFile(outputFilename, std::ios::out | std::ios::binary);
    //    if (outputFile.is_open()) {
    //        outputFile.write(decoded_string.c_str(), decoded_string.length());
    //        outputFile.close();
    //        return true;
    //    }
    //    else {
    //        __ERROR("Error opening file for writing!\n");
    //    }
    //    return false;
    //}

    // Convert std::string to LPCWSTR (createDirectory has LPCWSTR pathin as argument)
    //static LPCWSTR StringToLPCWSTR(const std::string& str) {
    //    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    //    if (size == 0) {
    //        // Handle error
    //        __ERROR("Error in MultiByteToWideChar: ", GetLastError(), "\n");
    //        return nullptr;
    //    }

    //    // Allocate buffer
    //    wchar_t* buffer = new wchar_t[size];

    //    // Convert the string
    //    if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size) == 0) {
    //        // Handle error
    //        __ERROR("Error in MultiByteToWideChar: ", GetLastError(), "\n");
    //        delete[] buffer;
    //        return nullptr;
    //    }

    //    return buffer;
    //}

    //***************************************************************************

    // Find boundary in received mail

    static std::string FindMailID(Ref<Socket> mail_receiver, int order_of_mail) {
        mail_receiver->Send("UIDL");
        std::string buffer = mail_receiver->Receive();

        int mail_amount = CountMailAmount(mail_receiver);
        // find the mail want to find id
        int id_line = 0;
        std::string id;
        std::stringstream buffer_stream(buffer);
        for (int i = 0; i < mail_amount; i++) {
            buffer_stream >> id_line;
            buffer_stream >> id;
            if (id_line == order_of_mail)
                break;
        }

        return id;
    }

    static std::string FindBoundary(const std::filesystem::path& path) {
        std::ifstream fi(path);

        if (!fi.is_open()) {
            __ERROR("Can't open ", path, "\n");
            return "";
        }

        std::string buffer;

        char delim = '\r';

        getline(fi, buffer, delim);
        std::string boundaryString = "boundary=\"";

        std::string boundary = buffer.substr(buffer.find(boundaryString) + boundaryString.size(), buffer.length());
        boundary.pop_back();

        fi.close();
        return boundary;
    }



    static std::string FindInfo(RetrievedMail& retreived_mail, const std::filesystem::path& path) {
        std::filesystem::path temporary_saving_mail_content_file = ".//temporary_saving_mail_content.txt";
        std::ifstream fi(temporary_saving_mail_content_file, std::ios::binary | std::ios::trunc);

        if (!fi.is_open()) {
            __ERROR("Can't open ", path, "\n");
            return "";
        }

        std::string buffer;
        std::string boundary = FindBoundary(path);
        std::string content = "";

        while (!fi.eof()) {
            std::getline(fi, buffer);
            if (buffer.find("From: ") != std::string::npos) {
                retreived_mail.Sender = buffer.substr(buffer.find(" ") + 1, buffer.length());
            }

            if (buffer.find("To: ") != std::string::npos) {
                buffer = buffer.substr(buffer.find(" ") + 1, buffer.length());
                while (!buffer.empty()) {
                    retreived_mail.Tos.push_back(buffer.substr(0, buffer.find(" ")));
                    buffer.erase(0, buffer.find(" ") + 1);
                }
            }

            if (buffer.find("Cc: ") != std::string::npos) {
                buffer = buffer.substr(buffer.find(" ") + 1, buffer.length());
                while (!buffer.empty()) {
                    retreived_mail.Ccs.push_back(buffer.substr(0, buffer.find(" ")));
                    buffer.erase(0, buffer.find(" ") + 1);
                }
            }

            if (buffer.find("Subject: ") != std::string::npos) {
                retreived_mail.Subject = buffer.substr(buffer.find(" ") + 1, buffer.length());
            }

            if (buffer.find("Content-Transfer-Encoding") != std::string::npos) {
                std::getline(fi, buffer);
                do {
                    std::getline(fi, buffer);
                    content += buffer;
                } while (buffer.find(boundary) == std::string::npos);
                retreived_mail.Content = content;
            }

            if (buffer.find("filename") != std::string::npos) {
                content = "";
                FileInfo file_info;
                file_info.FileName = buffer.substr(buffer.find("\"") + 1, buffer.length());
                file_info.FileName.pop_back();
                std::getline(fi, buffer);
                std::getline(fi, buffer);
                do {
                    content += buffer;
                    std::getline(fi, buffer);
                } while (buffer.find(boundary) == std::string::npos);
                file_info.Base64_Encoded_Content = content;
                retreived_mail.AttachedFiles.push_back(file_info);
            }
        }

        fi.close();
    }

    void RetreiveMailSFromServer(Ref<Socket>mail_receiver, Library& mail_container, const std::filesystem::path& mailbox_folder_path) {
        std::string buffer;

        int mail_amount = CountMailAmount(mail_receiver);

        std::vector<size_t> sizeOfMails(mail_amount, 0);

        mail_receiver->Send("LIST");
        buffer = mail_receiver->Receive();

        int size_line;
        std::stringstream ss(buffer);
        for (int i = 0; i < mail_amount; i++) {
            ss >> size_line;
            ss >> sizeOfMails[i];
        }

        std::filesystem::path temporary_saving_mail_content_file = ".//temporary_saving_mail_content.txt";
        for (int i = 1; i <= mail_amount; i++) {
            std::string id = FindMailID(mail_receiver, i) + ".msg";
            std::ofstream mail_file(mailbox_folder_path / id);
            std::ofstream fo(temporary_saving_mail_content_file, std::ios::binary | std::ios::trunc);
            mail_receiver->Send("RETR " + std::to_string(i));
            buffer = mail_receiver->Receive(sizeOfMails[i - 1]);
            fo.write(buffer.c_str(), buffer.length());
            mail_file.write(buffer.c_str(), buffer.length());

            fo.close();
            mail_file.close();
            RetrievedMail retreived_mail;
            FindInfo(retreived_mail, temporary_saving_mail_content_file);
            mail_container.AddNewMail(retreived_mail);
        }
        remove(temporary_saving_mail_content_file);
    }
}