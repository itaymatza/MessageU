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

enum class Status { server_info_error = -10, client_info_error, proper = 0 };

void getServerInfoFromFile(std::string* ip_address, std::string* port_number, Status* status);
void getClientInfoFromFile(std::string* client_name, uint8_t* uid, std::string* private_key, Status* status);
void writeMeInfoFile(std::string username, uint8_t uid[16], Status* status);
bool isFileExist(std::string filename);
uint32_t getFileSize(std::string file);
void writeResponsePayloadFromFile(boost::asio::ip::tcp::socket& sock, std::string filename, uint32_t total_size);

#endif /* __DATA_HELPER_H__ */