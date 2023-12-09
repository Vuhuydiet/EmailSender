#include "pch.h"
#include "SentMail.h"

void SentMail::AddTo(const std::string& to)
{
	if (to.empty())
		return;
	Tos.push_back(to);
}

void SentMail::AddCc(const std::string& cc)
{
	if (cc.empty())
		return;
	Ccs.push_back(cc);
}

void SentMail::AddBcc(const std::string& bcc)
{
	if (bcc.empty())
		return;
	Bccs.push_back(bcc);
}

void SentMail::AddContentLine(const std::string& text)
{
	Content.push_back(text);
}

void SentMail::AddAttachedFile(const std::filesystem::path& path)
{
	AttachedFilePaths.push_back(path);
}
