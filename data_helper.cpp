/**
	Data helper, functions related to DB and files.
	@file data_helper.cpp
	@author Itay Matza
	@version 1.0
*/
#include <fstream>
#include <iostream>
#include <locale>
#include "crypto.h"
#include "data_helper.h"

using namespace std;
constexpr int PUBKEY_LEN = 160;
constexpr int CHUNK_SIZE = 1024;

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
void getServerInfoFromFile(string* ip, string* port, Status* status){

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
void getClientInfoFromFile(string* client_name, uint8_t* uid, string* private_key, Status* status) {

	ifstream file("me.info");
	if (file) {
		string name, uidstring, key, buffer;
		if (!getline(file, name) || !getline(file, uidstring) || !getline(file, key)) {
			cerr << "Error: Illegal me.info file.\n";
			*status = Status::client_info_error;
		}
		while (file >> buffer) {
			key += buffer;
		}

		*client_name = name;
		HexStringTocharArray(uidstring, uid);
		*private_key = key;
		file.close();
	}
	else {
		cerr << "Error: Unable to open me.info file.\n";
		*status = Status::client_info_error;
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
	string buffer, key;
	ifstream ifile("me.info");
	while (ifile >> buffer) {
		key += buffer;
		key += "\n";
	}
	ifile.close();

	ofstream ofile("me.info");
	if (ofile) {
		ofile << username << endl;
		string uuid = charArrayToHexString(uid);
		ofile << uuid << endl;
		ofile << key << endl;
		ofile.close();
	}
	else {
		cerr << "Error: Unable to open and write me.info file.\n";
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
		cout << "Error: File not found." << endl;
		return false;
	}
}


// Counts each char of the file
uint32_t getFileSize(string file) {
	ifstream in_file(file, ios::binary);
	in_file.seekg(0, ios::end);
	uint32_t file_size = in_file.tellg();
	return file_size;
}


// Reads a file and writes to the given socket
void writeRequestPayloadFromFile(boost::asio::ip::tcp::socket& sock, string filename, uint32_t total_size) {
	FILE* file;
	char* payload_buffer = (char*)malloc(CHUNK_SIZE);
	if (!fopen_s(&file, filename.c_str(), "rb") && file != NULL && payload_buffer) {
		size_t total_sent = 0;
		while (total_sent < total_size) {
			size_t size = min(static_cast<size_t>(CHUNK_SIZE), (total_size - total_sent));
			size_t read = fread(payload_buffer, 1, size, file);
			size_t sent = boost::asio::write(sock, boost::asio::buffer(payload_buffer, read));
			total_sent += sent;
		}
		fclose(file);
		free(payload_buffer);
	}
	else {
		cerr << "Error: Unable to read payload file.\n";
	}
}


bool createTmpDirectory() {
	string dirname = "%TMP%";
	std::wstring_convert< std::codecvt<wchar_t, char, std::mbstate_t> > conv;
	std::wstring wstr = conv.from_bytes(dirname); // proper dirname type for CreateDirectory function
	if (!CreateDirectory(wstr.c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError()) {
		cout << "Error: Couldn't create " << dirname << " directory.\n";
		return false;
	}
	return true;
}


string genRandomString() {
	string tmp_s;
	int len = 32;
	static const char alphanum[] = //all chars for random string
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	srand((unsigned)time(NULL) * _getpid());
	tmp_s.reserve(len);
	for (int i = 0; i < len; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	return tmp_s;
}

string writeReceivedPayloadToFile(boost::asio::ip::tcp::socket& sock, uint32_t total_size) {
	FILE* file;
	string filename = "%TMP%\\" + genRandomString();
	char* payload_buffer = (char*)malloc(CHUNK_SIZE);
	if (!fopen_s(&file, filename.c_str(), "wb") && file != NULL && payload_buffer) {
		size_t payload_length = 0;
		while (payload_length < total_size) {
			size_t size = min(static_cast<size_t>(CHUNK_SIZE), (total_size - payload_length));
			size_t received = boost::asio::read(sock, boost::asio::buffer(payload_buffer, size));
			payload_length += received;
			fwrite(payload_buffer, 1, received, file);
			fflush(file);
		};
		fclose(file);
		free(payload_buffer);
		return filename;
	}
	else {
		return "Error: Unable to save the file.";
	}
}


string writeFileAsEncrypted(std::string filename, uint8_t symmetric_key[16]) {
	ifstream ifile(filename);
	string ofilename = "";

	if (ifile) {
		string ofilename = ".encrypted" + filename + genRandomString();
		
		ofstream ofile(ofilename);
		if (ofile) {
			string buffer;
			while (getline(ifile, buffer)) {
				string ciphertext = encryptAesString(symmetric_key, buffer);
				ofile << ciphertext << endl;
			}
			ofile.close();
			return ofilename;
		}
		else {
			cout << "Error: Unable to open and write encrypted file.\n";
		}
		ifile.close();
	}
	else {
		cout << "Error: Unable to open " + filename + "file.\n";
	}
	return ofilename;
}


string writeFileAsDecrypted(std::string filename, uint8_t symmetric_key[16]) {
	ifstream ifile(filename);
	string ofilename = "";

	if (ifile) {
		string ofilename = "%TMP%\\" + genRandomString();

		ofstream ofile(ofilename);
		if (ofile) {
			string buffer;
			while (ifile >> buffer) {
				string ciphertext = decryptAesString(symmetric_key, buffer);
				ofile << ciphertext;
			}
			ofile.close();
			return ofilename;
		}
		else {
			cout << "Error: Unable to open and write encrypted file.\n";
		}
		ifile.close();
	}
	else {
		cout << "Error: Unable to open " + filename + "file.\n";
	}
	return ofilename;
}


void deleteFile(string filename) {
	std::wstring_convert< std::codecvt<wchar_t, char, std::mbstate_t> > conv;
	std::wstring wstr = conv.from_bytes(filename); // proper dirname type for DeleteFile function
	if (!DeleteFile(wstr.c_str())) {
		cerr << "Error: Couldn't delete requested file.\n";
	}
}