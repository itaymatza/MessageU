/**
	Data helper, functions related to DB and files.
	@file files_helper.h
	@author Itay Matza
	@version 1.0 01/03/21
*/
#ifndef __DATA_HELPER_H__
#define __DATA_HELPER_H__

constexpr int CHUNK_SIZE = 1024;

void getServerInfo(std::string* ip_address, std::string* port_number);
void getClientInfo(string* clien_name, std::vector<unsigned char>* uid);

#endif /* __DATA_HELPER_H__ */