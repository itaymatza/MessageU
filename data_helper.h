/**
	Data helper, functions related to DB and files.
	@file files_helper.h
	@author Itay Matza
	@version 1.0 01/03/21
*/
#ifndef __DATA_HELPER_H__
#define __DATA_HELPER_H__

#include <vector>
#include <boost/asio.hpp>

void getServerInfoFromFile(std::string* ip_address, std::string* port_number, std::string info_file);
void getClientInfoFromFile(std::string* client_name, uint8_t* uid, std::string* private_key, std::string info_file);
void writeMeInfoFile(std::string username, uint8_t uid[16]);
bool isFileExist(std::string filename);
uint32_t getFileSize(std::string file);
void writeRequestPayloadFromFile(boost::asio::ip::tcp::socket& sock, std::string filename, uint32_t total_size);
std::string writeReceivedPayloadToFile(boost::asio::ip::tcp::socket& sock, uint32_t total_size);
bool createTmpDirectory();
std::string genRandomString();
void deleteFile(std::string filename);

#endif /* __DATA_HELPER_H__ */