#pragma once

#include <filesystem>

const std::filesystem::path _DEFAULT_CONFIG_DIR					= "Config";
const std::filesystem::path _DEFAULT_HOST_MAILBOX_DIR			= "MSG";
const std::filesystem::path _DEFAULT_DOWNLOAD_DIR				= "Downloads";

const std::filesystem::path _DEFAULT_APP_CONFIG_PATH			= _DEFAULT_CONFIG_DIR / "Config.yml";
const std::filesystem::path _DEFAULT_MAIL_FILTER_CONFIG_PATH	= _DEFAULT_CONFIG_DIR / "DefaultMailFilterConfig.yml";


#define _SERVER_DEFAULT_IP			"127.0.0.1"
#define _SMTP_DEFAULT_PORT			2500
#define _POP3_DEFAULT_PORT			1100
