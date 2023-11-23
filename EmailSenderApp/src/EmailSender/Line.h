#pragma once

#include <string>

struct Line
{
	enum class Type {
		None, 
		Server, Client, App,
		Info, Warn, Err
	};
	
	Type type = Type::None;
	std::string header;
	std::string msg;

	Line() = default;
	Line(Type _ty) : type(_ty) {}
	Line(Type _ty, const std::string& _msg) : type(_ty), header(TypeToHeader(_ty)), msg(_msg) {}
	Line(Type _ty, const std::string& _header, const std::string& _msg) : type(_ty), header(_header), msg(_msg) {}

	void Display() const;

private:
	std::string TypeToHeader(Type ty) const {
		switch (ty)
		{
		case Type::Server: return "Server";
		case Type::Client: return "Client";
		case Type::App: return "App";
		case Type::Info: return "INFO";
		case Type::Warn: return "WARNING";
		case Type::Err: return "ERROR";
		}
		return "Unknown";
	}
};
