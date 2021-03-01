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
		size_t position = info.find(":");
		*ip = info.substr(0, position);
		position += 1;
		*port = info.substr(position);

		int port_int = atoi((*port).c_str());
		if (!is_number(*port) || !is_end_of_file.empty() || port_int < 1 || port_int > 65535) {
			cerr << "Error: invalid file, the file should contain just valid port number.\n";
			*status = Status::server_info_error;
		}
		file.close();
	}
	else {
		cerr << "Error: Unable to open server.info file.\n";
		*status = Status::server_info_error;
	}
}

unsigned char parse_hex(char c)
{
	if ('0' <= c && c <= '9') return c - '0';
	if ('A' <= c && c <= 'F') return c - 'A' + 10;
	if ('a' <= c && c <= 'f') return c - 'a' + 10;
	std::abort();
}

void HexStringTocharArray(string str, uint8_t* uid) {
	size_t length = str.length() / 2;

	for (size_t i = 0, j = 0; i < length; i++) {
		uid[i] = 16 * parse_hex(str[2 * i]) + parse_hex(str[2 * i + 1]);
	}
}


/**
	Reads client's name and uid from ./me.info file.
	If invalid info file format - output stream for errors and exit.
	Writes the info to n and port string parameters.
*/
void getClientInfo(string* clien_name, uint8_t* uid, Status* status) {

	ifstream file("me.info");
	if (file) {
		string name, uidstring, is_end_of_file;
		getline(file, name);
		getline(file, uidstring);
		if (getline(file, is_end_of_file)) {
			cerr << "Error: Illegal me.info file.\n";
			*status = Status::client_info_error;
		}

		*clien_name = name;
		HexStringTocharArray(uidstring, uid);
		file.close();
	}
	else {
		cerr << "Error: Unable to open me.info file.\n";
		*status = Status::client_info_error;
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

string charArrayToHexString(uint8_t ch[]) {
	string out;
	size_t length = sizeof(ch) * 2;

	for (int i = 0; i < length; i++) {
		char buffer[3];
		sprintf_s(buffer, "%02X", ch[i]);
		out += buffer;
	}
	return out;
}

void writeMeInfoFile(std::string username, uint8_t uid[16], Status* status) {
	ofstream file("me.info");
	if (file) {
		file << username << endl;
		string uuid = charArrayToHexString(uid);
		file << uuid << endl;
		file.close();
	}
	else {
		cerr << "Error: Unable to open and write me.info file.\n";
		*status = Status::client_info_error;
	}
}