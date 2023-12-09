#pragma once

#include "Core/Socket.h"
#include "Email/SentMail.h"

namespace SMTP {

	void SendMail(Ref<Socket> socket, const SentMail& mail);

}