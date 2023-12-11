#include "pch.h"
#include "RetrievedMail.h"
#include "Base64/Base64.h"
#include "Core/Format.h"

std::string RetrievedMail::ToString() const {
	std::string res;

	res +=  FMT::format("From: {}\n", this->Sender);
	
	res += "To: ";
	for (const auto& to : this->Tos) {
		res += to + ", ";
	}
	res.pop_back();
	res.pop_back();
	res += "\n";
	
	res += "Cc: ";
	for (auto cc : this->Ccs) {
		res += cc + ", ";
	}
	res.pop_back();
	res.pop_back();
	res += "\n";

	res += FMT::format("Subject: {}\n",this->Subject);

	res += FMT::format("Content:\n {}\n", this->Content);

	res += FMT::format("Attached-File-Amount: {}\n", this->AttachedFiles.size());

	for (int i = 0; i < this->AttachedFiles.size(); i++) {
		res += FMT::format("{}. {}\n", i + 1, this->AttachedFiles[i].FileName);
	}

	return res;
}

void RetrievedMail::SaveFile(const std::string& file_name, const std::filesystem::path& path) const {
	std::ofstream fo(path / file_name, std::ios::binary);

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