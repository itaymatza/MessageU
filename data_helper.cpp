/**
	Data helper, functions related to DB and files.
	@file data_helper.cpp
	@author Itay Matza
	@version 1.0
*/
#include <fstream>
#include <iostream>
#include "data_helper.h"
#include <boost/asio.hpp>

using namespace std;

// Checks and returns if a given string is a number representation.
bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}


/**
	Reads server's IP address and TCP port number from ./server.info file.
	If invalid info file format or port number - output stream for errors and exit.
	Writes the info to ip and port string parameters.
*/
void getServerInfo(string* ip, string* port, Status* status){

	ifstream file("server.info");
	if (file) {
		string info, is_end_of_file;

		file >> info >> is_end_of_file;
		int position = info.find(":");
		*ip = info.substr(0, position);
		position += 1;
		*port = info.substr(position);

		int port_int = atoi((*port).c_str());
		if (!is_number(*port) || !is_end_of_file.empty() || port_int < 1 || port_int > 65535) {
			cerr << "Error: invalid file, the file should contain just valid port number.\n";
			*status = server_info_error;
		}
		file.close();
	}
	else {
		cerr << "Error: Unable to open server.info file.\n";
		*status = server_info_error;
	}
}

unsigned char parse_hex(char c)
{
	if ('0' <= c && c <= '9') return c - '0';
	if ('A' <= c && c <= 'F') return c - 'A' + 10;
	if ('a' <= c && c <= 'f') return c - 'a' + 10;
	std::abort();
}

std::vector<unsigned char> parse_string(const std::string& s)
{
	if (s.size() % 2 != 0) std::abort();
	std::vector<unsigned char> result(s.size() / 2);

	for (std::size_t i = 0; i != s.size() / 2; ++i)
		result[i] = 16 * parse_hex(s[2 * i]) + parse_hex(s[2 * i + 1]);

	return result;
}


/**
	Reads client's name and uid from ./me.info file.
	If invalid info file format - output stream for errors and exit.
	Writes the info to n and port string parameters.
*/
void getClientInfo(string* clien_name, std::vector<unsigned char>* uuid, Status* status) {

	ifstream file("me.info");
	if (file) {
		string first_name, last_name, uid, is_end_of_file;

		file >> first_name >> last_name >> uid >> is_end_of_file;
		*clien_name = first_name + " " + last_name;
		*uuid = parse_string(uid);

		file.close();
	}
	else {
		cerr << "Error: Unable to open server.info file.\n";
		*status = client_info_error;
	}
}

bool isFileExist(string filename) {
	FILE* file;
	if (!fopen_s(&file, filename.c_str(), "rb") && file != NULL) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}