#include "pch.h"
#include "POP3.h"

namespace POP3 {

    // File interacting functions
    static void ReceiveServerGreeting() {
        std::string buffer;
        mail_receiver->Receive(buffer);
        __INFO(buffer);
    }

    void ConnectServer(const std::string& ip, int port) {
        mail_receiver->Connect(ip, port);
        ReceiveServerGreeting();
    }

    void DisconnectServer() {
        mail_receiver->Disconnect();
    }

    void LoginServer(const std::string& user_name, const std::string& password) {
        std::string userCommand = "USER " + user_name;
        std::string buffer = "";
        mail_receiver->Send(userCommand);
        mail_receiver->Receive(buffer);
        __INFO(buffer, "\n");

        std::string passCommand = "PASS " + password;
        mail_receiver->Send(passCommand);
        mail_receiver->Receive(buffer);
        __INFO(buffer, "\n");
    }

    std::string FindMailID(int number_of_mail) {
        std::string buffer = "";
        mail_receiver->Send("UIDL");
        mail_receiver->Receive(buffer);

        // find the mail want to find id
        std::string id_line = "";
        char delim = '\r';
        
        std::stringstream buffer_stream(buffer);
        while (getline(buffer_stream, id_line, delim)) {
            if (id_line[0] == (number_of_mail - '0')) break;
        }

        int space_pos = id_line.size() - 1;
        while (id_line[space_pos] != ' ') space_pos--;
        std::string id = buffer.substr(space_pos, id_line.size());
        if (id[id.size() - 1] == '\r\n' || id[id.size() - 1] == '\r' || id[id.size() - 1] == '\n') id.pop_back();
        return id;
    }

    void ListMail() {
        std::string buffer = "";
        mail_receiver->Send("LIST");
        mail_receiver->Receive(buffer);
        __INFO(buffer, "\n");
    }

    void RetreiveMail(int number_of_mail, const std::string& mail_file_path) {
        // Fetch individual emails (e.g., for email with sequence number 1)
        mail_receiver->Send("RETR " + std::to_string(number_of_mail));

        // Save mail content to file

        std::ofstream temporary_file(".\\temp_file.txt", std::ios::binary);

        if (!temporary_file.is_open()) {
            std::cerr << "Error creating/opening file" << std::endl;
            mail_receiver->Disconnect();
            return;
        }

        std::string buffer;
        do {
            mail_receiver->Receive(buffer);
            temporary_file.write(buffer.c_str(), buffer.size());
            if (buffer[buffer.size() - 3] == '.') break;
            
        } while (!buffer.empty());

        temporary_file.close();
        std::string mail_id = FindMailID(number_of_mail);
        std::ofstream mail_file(mail_file_path + "\\" + mail_id, std::ios::binary);
        std::ifstream tempFile(".\\temp_file.txt", std::ios::binary);
        std::string tempBuffer;

        std::getline(tempFile, tempBuffer); // ignore the first line
        while (!temporary_file.eof()) {
            getline(tempFile, tempBuffer);
            if (tempBuffer == ".\r") break;
            mail_file << tempBuffer;
        }
        tempFile.close();
        remove(".\\temp_file.txt");

        mail_file.close();
    }
	// Helpers

    // Function to decode Base64 string
    static std::string Base64_Decode(const std::string& encoded_string) {
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

    // Decode and save decoded file
    static bool SaveDecodedFile(const std::string& encoded_string, const std::string& outputFilename) {
        std::string decoded_string = Base64_Decode(encoded_string);

        // Write the decoded data to a file
        std::ofstream outputFile(outputFilename, std::ios::out | std::ios::binary);
        if (outputFile.is_open()) {
            outputFile.write(decoded_string.c_str(), decoded_string.length());
            outputFile.close();
            return true;
        }
        else {
            __ERROR("Error opening file for writing!\n");
        }
        return false;
    }
    
    // Find boundary in received mail

    static std::string FindBoundary(const std::string& mail_file_path) {
        std::ifstream fi(mail_file_path);

        if (!fi.is_open()) {
            __ERROR("Can't open ", mail_file_path, "\n");
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

    // Convert std::string to LPCWSTR (createDirectory has LPCWSTR pathin as argument)
    static LPCWSTR StringToLPCWSTR(const std::string& str) {
        int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        if (size == 0) {
            // Handle error
            __ERROR("Error in MultiByteToWideChar: ", GetLastError(), "\n");
            return nullptr;
        }

        // Allocate buffer
        wchar_t* buffer = new wchar_t[size];

        // Convert the string
        if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size) == 0) {
            // Handle error
            __ERROR("Error in MultiByteToWideChar: ", GetLastError(), "\n");
            delete[] buffer;
            return nullptr;
        }

        return buffer;
    }

    // Utility function

    // File interacting functions

    bool HasAttachedFile(const std::string& file_nam_path) {
        std::ifstream fi(file_nam_path);
        if (!fi.is_open()) {
            __ERROR("Can't open ", file_nam_path, "\n");
            return false;
        }
        std::string buffer = "";
        while (!fi.eof()) {
            std::getline(fi, buffer);
            if (buffer.find("filename") != std::string::npos) return true;
        }

        fi.close();
        return false;
    }

    int CountAttachedFile(const std::string& mail_file_path) {
        std::ifstream fi(mail_file_path);

        if (!fi.is_open()) {
            __ERROR("Can't open ", mail_file_path, "\n");
            return -1;
        }

        std::string buffer;
        int count = 0;
        char delim = '\r';
        while (!fi.eof()) {
            std::getline(fi, buffer, delim);
            if (buffer.find("filename") != std::string::npos) count++;
        }

        fi.close();
        return count;
    }

    std::string FindReceivedMail(const std::string& mail_file_path) {
        std::ifstream fi(mail_file_path);

        if (!fi.is_open()) {
            __ERROR("Can't open ", mail_file_path, "\n");
            return "";
        }

        std::string buffer;

        char delim = '\r';
        std::string to_str_const = "To: ";
        while (!fi.eof()) {
            getline(fi, buffer, delim);
            if (buffer.find(to_str_const) != std::string::npos) break;
        }

        std::string received_mail = buffer.substr(buffer.find(to_str_const) + to_str_const.size(), buffer.length());

        fi.close();
        return received_mail;
    }

    void SaveAttachedFile(const std::string& saved_attached_file,const std::string& mail_file_path) {
        if (!HasAttachedFile(mail_file_path)) {
            __ERROR("This mail doesn't have attached file!\n");
            return;
        }

        std::string received_mail = FindReceivedMail(mail_file_path);

        if (received_mail.empty()) {
            __ERROR("Can't find received mail\n");
        }

        std::string default_saving_dir = "";
        int delimPos = mail_file_path.size() - 1; // delim == '\\'
        
        while (mail_file_path[delimPos] != '\\') { delimPos--; }

        default_saving_dir = mail_file_path.substr(0, delimPos + 1) + received_mail;

        // Get saving dir
        std::cout << "Enter directory to save attached file: \n";
        std::cout << "If using default director: " << default_saving_dir << ", press Enter.";
        std::string saving_dir = "";
        getline(std::cin, saving_dir);
        if (saving_dir.empty()) saving_dir = default_saving_dir;
        else saving_dir += "\\" + received_mail;
        // CreateDirectory get LPCWSTR pathin as argument
        LPCWSTR LPCWSTR_saving_dir = StringToLPCWSTR(saving_dir);
        bool isCreated = CreateDirectory(LPCWSTR_saving_dir, NULL);

        if (!isCreated) {
            __ERROR("Can't find director\n");
            return; 
        }

        std::ifstream fi(mail_file_path);

        if (!fi.is_open()) {
            __ERROR("Can't open ", mail_file_path, "\n");
            return;
        }

        saving_dir += "\\" + saved_attached_file;
        std::ofstream fo(saving_dir);

        if (!fo.is_open()) { 
            __ERROR("Can't open ", saving_dir, "\n");
            
            fi.close();
            return;
        }
        std::string buffer;
        bool found_file_content = false;
        std::string boundary = FindBoundary(mail_file_path);

        if (boundary.empty()) {
            __ERROR("Can't find boundary\n");

            fo.close();
            fi.close();
            return;
        }

        char delim = '\r';
        std::string encodedString = "";
        while (!fi.eof()) {
            getline(fi, buffer, delim);

            if (found_file_content && buffer.find(boundary) != std::string::npos) break;

            if (found_file_content) encodedString += buffer;

            if (!found_file_content && buffer.find(saved_attached_file) != std::string::npos) {
                found_file_content = true;
                getline(fi, buffer, delim);
                getline(fi, buffer, delim);
                getline(fi, buffer, delim);
            }
        }
        
        if (!SaveDecodedFile(encodedString, saving_dir)) {
            __ERROR("Can't save attached file\n");
        }


        fo.close();
        fi.close();
    }
}