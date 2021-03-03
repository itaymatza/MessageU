/**
	Data helper, functions related to DB and files.
	@file files_helper.h
	@author Itay Matza
	@version 1.0 01/03/21
*/
#include <vector>

#ifndef __DATA_HELPER_H__
#define __DATA_HELPER_H__

enum class Status { server_info_error = -10, client_info_error, proper = 0 };

void getServerInfoFromFile(std::string* ip_address, std::string* port_number, Status* status);
void getClientInfoFromFile(std::string* clien_name, uint8_t* uid, Status* status, uint8_t public_key[]);
bool isFileExist(std::string filename);
void writeMeInfoFile(std::string username, uint8_t uid[16], Status* status);

#endif /* __DATA_HELPER_H__ */