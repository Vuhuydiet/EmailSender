#include "pch.h"
#include "SMTP.h"
#include "Format.h"
#include "Base64/Base64.h"

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

	static std::string CreateMessageId(const std::string& sender, const Date& date) {
		// Output the current time
		std::string id = "<";
		char delim = '-';
		id += base64::Base64_Encode("0" + std::to_string(date.day) + "th") + delim;
		id += base64::Base64_Encode(std::to_string(date.hour)) + delim;
		id += base64::Base64_Encode(std::to_string(date.min)) + delim;
		id += base64::Base64_Encode(std::to_string(date.sec)) + delim;
		
		// get user name from address mail
		size_t atPos = sender.find('@');
		if (atPos != std::string::npos) {
			id += base64::Base64_Encode(sender.substr(0, atPos));
		}
		else { // get all address mail
			id += base64::Base64_Encode(sender);
		}		
		return id + "@gmail.com>";
	}

	std::string CreateBoundary(const Date& date){
		// Output the current time
		std::string id = "";
		id = "------------";
		id += base64::Base64_Encode(std::to_string(date.hour));
		id += base64::Base64_Encode(std::to_string(date.min));
		id += base64::Base64_Encode(std::to_string(date.sec));
		return id;
	}

	static std::string CreateDateMail(const Date& date) {
		std::string DateMail;
		DateMail += std::to_string(date.day) + "/";
		DateMail += std::to_string(date.month) + "/";
		DateMail += std::to_string(date.year) + "  ";
		DateMail += std::to_string(date.hour) + ":";
		DateMail += std::to_string(date.min) + ":";
		DateMail += std::to_string(date.sec);
		return DateMail;
	}

	bool SendMail(Ref<Socket> socket, const SentMail& mail) {
		if (!socket->IsConnected()) {
			__ERROR("Socket is not connected, can not send the email!");
			return false;
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
		Date date;
		const std::string boundary = CreateBoundary(date);
		std::string Msg_ID = CreateMessageId(mail.Sender,date);
		std::string DateMail = CreateDateMail(date);

		// Send Header
		if (!mail.AttachedFilePaths.empty()) {
			socket->Send("Content-Type: multipart/mixed;boundary=\"" + boundary + "\"");
		}
		socket->Send("Message-ID: "+ Msg_ID);
		socket->Send("DATE: " + DateMail);
		socket->Send("MIME-Version: 1.0");
		socket->Send("User-Agent: C++ Client");
		socket->Send("Content-Langaue: en-US");
		socket->Send(FMT::format("From: <{}>", mail.Sender));
		socket->Send(FMT::format("To: {}", AddressListToString(mail.Tos)));
		socket->Send(FMT::format("Cc: {}", AddressListToString(mail.Ccs)));
		socket->Send(FMT::format("Subject: {}", mail.Subject));
		

		if (!mail.AttachedFilePaths.empty()) {
			socket->Send("");
			socket->Send("This is a multi-part message in MIME format.");
			socket->Send("--" + boundary);
		}
		
		// Header of content
		socket->Send("Content-Type:text/plain;charest=UTF-8;format=flowed");
		socket->Send("Content-Transfer-Encoding: 7bit");
		socket->Send("");

		// Content
		for (const auto& line : mail.Content) {
			socket->Send(line);
		}
		socket->Send("");

		// Attached files
		for (const auto& path : mail.AttachedFilePaths) {
			socket->Send("--" + boundary);
			SendFile(socket, path);
			socket->Send("");
		}

		if (!mail.AttachedFilePaths.empty()) {	
			socket->Send("--" + boundary +"--");
		}

		// End sending data
		socket->Send(".");
		socket->Receive();
		return true;
	}

}