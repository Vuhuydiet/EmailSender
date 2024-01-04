#include "pch.h"
#include "RetrievedMail.h"
#include "Base64/Base64.h"
#include "Core/Format.h"
#include "Utils/Utils.h"
#include "Debug/Debug.h"


static std::string FindBoundary(const std::string& first_line) {
	const std::string boundaryString = "boundary=\"";
	size_t boundary_pos = first_line.find(boundaryString);
	if (boundary_pos == std::string::npos)
		return "";
	std::string boundary = first_line.substr(boundary_pos + boundaryString.size());
	boundary.pop_back();
	return boundary;
}

RetrievedMail::RetrievedMail(const std::filesystem::path& msg_path) {
	Id = msg_path.filename().string();

	std::ifstream in(msg_path);

	std::string line;
	std::getline(in, line);
	std::string boundary = FindBoundary(line);
	if (boundary.empty())
		boundary = ".";

	std::string content;
	do {
		if (line.find("Date: ") != std::string::npos) {
			SendDate = Date(line.substr(line.find(" ") + 1));
		}

		if (line.find("From: ") != std::string::npos) {
			Sender = line.substr(line.find(" ") + 1);
		}

		if (line.find("To: ") != std::string::npos) {
			line = line.substr(line.find(" ") + 1);
			RemoveChar(line, ' ');
			Tos = Split(line, ',');
		}

		if (line.find("Cc: ") != std::string::npos) {
			line = line.substr(line.find(" ") + 1);
			RemoveChar(line, ' ');
			Ccs = Split(line, ',');
		}

		if (line.find("Subject: ") != std::string::npos) {
			Subject = line.substr(line.find(" ") + 1);
		}

		if (line.find("Content-Transfer-Encoding") != std::string::npos) {
			// Ignore the blank line
			std::getline(in, line);

			while (std::getline(in, line) && line.find(boundary) == std::string::npos) {
				content += line + '\n';
			}
			while (!content.empty() && content.back() == '\n')
				content.pop_back();
			Content = content;
		}

		if (line.find("filename") != std::string::npos) {
			content = "";
			FileInfo file_info;
			file_info.FileName = line.substr(line.find("\"") + 1);
			file_info.FileName.pop_back();
			std::getline(in, line);
			std::getline(in, line);
			std::getline(in, line);
			do {
				content += line;
				std::getline(in, line);
			} while (line.find(boundary) == std::string::npos);
			file_info.Base64_Encoded_Content = content;
			AttachedFiles.push_back(file_info);
		}
	} while (std::getline(in, line));
}

RetrievedMail::RetrievedMail(RetrievedMail&& other) noexcept {
	Sender = std::move(other.Sender);
	Tos = std::move(other.Tos);
	Ccs = std::move(other.Ccs);

	Subject = std::move(other.Subject);
	Content = std::move(other.Content);
	AttachedFiles = std::move(other.AttachedFiles);
}

RetrievedMail& RetrievedMail::operator=(RetrievedMail&& other) noexcept {
	Sender = std::move(other.Sender);
	Tos = std::move(other.Tos);
	Ccs = std::move(other.Ccs);

	Subject = std::move(other.Subject);
	Content = std::move(other.Content);
	AttachedFiles = std::move(other.AttachedFiles);
	return *this;
}

std::string RetrievedMail::ToString() const {
	std::string res;

	res +=  FMT::format("From: {}\n", this->Sender);
	
	res += "To: ";
	for (const auto& to : this->Tos) {
		res += to;
		if (to != Tos.back()) 
			res += ", ";
	}
	res += "\n";
	
	res += "Cc: ";
	for (const auto& cc : this->Ccs) {
		res += cc;
		if (cc != Ccs.back())
			res += ", ";
	}
	res += "\n";

	res += FMT::format("Subject: {}\n",this->Subject);

	res += FMT::format("\n{}\n\n", this->Content);

	//if (AttachedFiles.empty()) {
	//	res += "This email does not contain attached files.\n";
	//	return res;
	//}

	//res += FMT::format("Number of attached files: {}\n", this->AttachedFiles.size());

	//for (int i = 0; i < this->AttachedFiles.size(); i++) {
	//	res += FMT::format("{}. {}\n", i + 1, this->AttachedFiles[i].FileName);
	//}

	return res;
}

void RetrievedMail::SaveFile(const std::string& file_name, const std::filesystem::path& path) const {
	std::ofstream fo(path / file_name, std::ios::binary);

	if (!fo.is_open()) {
		__ERROR("Can't open ", path, "\n");
		return;
	}

	for (const auto& file : this->AttachedFiles) {
		if (file.FileName == file_name) {
			std::string decoded_string = base64::Decode(file.Base64_Encoded_Content);
			fo.write(decoded_string.c_str(), decoded_string.length());
			break;
		}
	}

	fo.close();
}

void RetrievedMail::SaveFile(int id, const std::filesystem::path& dir) const {
	const auto& file = this->AttachedFiles[id];
	std::string decoded_string = base64::Decode(file.Base64_Encoded_Content);
	std::ofstream fo(dir / file.FileName, std::ios::binary);
	fo.write(decoded_string.c_str(), decoded_string.length());
	fo.close();
}

void RetrievedMail::SaveAllFiles(const std::filesystem::path& dir) const {
	for (const auto& file : this->AttachedFiles) {
		std::string decoded_string = base64::Decode(file.Base64_Encoded_Content);
		std::ofstream fo(dir / file.FileName, std::ios::binary);
		fo.write(decoded_string.c_str(), decoded_string.length());
		fo.close();
	}
}