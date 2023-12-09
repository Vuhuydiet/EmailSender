#include "pch.h"
#include "SMTP.h"

#include "Format.h"

namespace SMTP {

	static std::string AddressListToString(const std::vector<std::string>& addr) {
		std::string res;
		for (int i = 0; i < addr.size(); i++) {
			res += FMT::format("<{}>", addr[i]);
			if (i != addr.size() - 1)
				res += ", ";
		}
		return res;
	}
	std::vector<int> CreateDate() {
		// Get the current time point
		std::time_t currentTime = std::time(nullptr);

		std::tm* localTime = std::localtime(&currentTime);
	
		// Extract date and time components
		std::vector<int> date;
		
		date.push_back(localTime->tm_mday);
		date.push_back(localTime->tm_mon + 1);
		date.push_back(localTime->tm_year + 1900);
		date.push_back(localTime->tm_hour);
		date.push_back(localTime->tm_min);
		date.push_back(localTime->tm_sec);

		return date;
	}

	std::string CreateMessageId(std::string sender) {
		std::vector<int> currentDate = CreateDate();
		// Output the current time
		std::string id ="<";
		char delim = '-';
		id += base64_encode("0" + std::to_string(currentDate[2]) + "th") + delim;
		for (int i = 3; i < currentDate.size(); i++) {
			id += base64_encode(std::to_string(currentDate[i])) + delim;
		}
		// get user name from address mail
		size_t atPos = sender.find('@');
		if (atPos != std::string::npos) {
			id += base64_encode(sender.substr(0, atPos));
		}
		else { // get all address mail
			id += base64_encode(sender);
		}		
		return id+"@gmail.com>";
	}

	std::string CreateBoundary(){
		// Output the current time

		std::vector<int> currentDate = CreateDate();
		// Output the current time
		std::string id;
		for (int i = 3; i < currentDate.size(); i++) {
			id += base64_encode(std::to_string(currentDate[i]));
		}
		return id;
	}

	std::string CreateDateMail() {
		std::vector<int> DateMailInit = CreateDate();
		std::string DateMail;
		for (int i = 0; i < DateMailInit.size(); i++) {
			if (i < 3) {
				DateMail += std::to_string(DateMailInit[i]) + "/";
				if (i == 2) DateMail += " ";
			}
			else DateMail += std::to_string(DateMailInit[i]) + ":";

		}
		return DateMail;
	}

	void SendMail(Ref<Socket> socket, const SentMail& mail) {
		if (!socket->IsConnected()) {
			__ERROR("Socket is not connected, can not send the email!");
			return;
		}

		socket->Send("HELO <EmailSenderApp>");
		socket->Receive();

		// Sender
		socket->Send(FMT::format("MAIL FROM: <{}>", mail.Sender));
		socket->Receive();

		// To, Cc, Bcc
		for (const auto& to : mail.Tos) {
			socket->Send(FMT::format("RCPT TO: <{}>", to));
			socket->Receive();
		}
		for (const auto& cc : mail.Ccs) {
			socket->Send(FMT::format("RCPT TO: <{}>", cc));
			socket->Receive();
		}
		for (const auto& bcc : mail.Bccs) {
			socket->Send(FMT::format("RCPT TO: <{}>", bcc));
			socket->Receive();
		}

		// Start sending data
		socket->Send("DATA");
		socket->Receive();

		// Header
		// Initialize
		const std::string boundary = CreateBoundary();
		std::string Msg_ID = CreateMessageId(mail.Sender);
		std::string DateMail = CreateDateMail();

		// Send Header
		if (!mail.AttachedFilePaths.empty()) {
			socket->Send("Content-Type: multipart/mixed;boundary=\"" + boundary + "\"");
		}
		socket->Send("Message ID: "+ Msg_ID);
		socket->Send("DATE: " + DateMail);
		socket->Send("MIME-Version: 1.0");
		socket->Send("User - Agent: C++ Client");
		socket->Send("Content-Langaue: en-US");
		socket->Send(FMT::format("From: <{}>", mail.Sender));
		socket->Send(FMT::format("To: {}", AddressListToString(mail.Tos)));
		socket->Send(FMT::format("Cc: {}", AddressListToString(mail.Ccs)));
		socket->Send(FMT::format("Subject: {}", mail.Subject));
		

		if (!mail.AttachedFilePaths.empty()) {
			socket->Send("\n");
			socket->Send("This is a multi-part message in MIME format.");
			socket->Send("--" + boundary);
		}
		
		// Header of content
		socket->Send("Content-Type:text/plain;charest=UTF-8;format=flowed");
		socket->Send("Content-Transfer-Encoding: 7bit");
		socket->Send("\n");

		// Content
		for (const auto& line : mail.Content) {
			socket->Send(line);
		}
		socket->Send("\n");

		// Attach file
		if (!mail.AttachedFilePaths.empty()) {
			for (int i = 0; i < mail.AttachedFilePaths.size(); i++) {
				socket->Send("--" + boundary);
				SendFile(socket, mail.AttachedFilePaths[i]);
				socket->Send("\n");
			}
			socket->Send("--" + boundary);
		}

		// End sending data
		socket->Send(".");
		socket->Receive();

		// Quit
		//socket->Send("QUIT");
		//socket->Receive();
	}

}