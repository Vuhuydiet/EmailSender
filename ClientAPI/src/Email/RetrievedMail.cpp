#include "pch.h"
#include "RetrievedMail.h"
#include "Base64/Base64.h"

std::string RetrievedMail::ToString() const {
	std::stringstream ss;

	ss << "From: " << this->Sender << "\n\n";
	
	ss << "To: ";
	for (auto to : this->Tos) {
		ss << to;
	}
	ss << "\n\n";

	ss << "Cc: ";
	for (auto cc : this->Ccs) {
		ss << cc;
	}
	ss << "\n\n";

	ss << "Subject: " << this->Subject << "\n\n";

	ss << "Content: \n\n" << this->Content << "\n\n";

	ss << "Attached-File-Amount: " << this->AttachedFiles.size() << "\n\n";

	for (int i = 0; i < this->AttachedFiles.size(); i++) {
		ss << i + 1 << " File-Name: " << this->AttachedFiles[i].FileName << "\n\n";
	}

	return ss.str();
}

void RetrievedMail::SaveFile(const std::string& file_name, const std::filesystem::path& path) {
	std::ofstream fo(path / file_name);

	if (!fo.is_open()) {
		__ERROR("Can't open ", path, "\n");
		return;
	}

	for (auto file : this->AttachedFiles) {
		if (file.FileName == file_name) {
			std::string decoded_string = base64::Base64_Decode(file.Base64_Encoded_Content);
			fo.write(decoded_string.c_str(), decoded_string.length());
			break;
		}
	}

	fo.close();
}