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
		socket->Send(FMT::format("From: <{}>", mail.Sender));
		socket->Send(FMT::format("To: {}", AddressListToString(mail.Tos)));
		socket->Send(FMT::format("Cc: {}", AddressListToString(mail.Ccs)));
		socket->Send(FMT::format("Subject: {}", mail.Subject));
		
		socket->Send("");

		// Content
		for (const auto& line : mail.Content) {
			socket->Send(line);
		}

		// End sending data
		socket->Send(".");
		socket->Receive();

		// Quit
		//socket->Send("QUIT");
		//socket->Receive();
	}

}