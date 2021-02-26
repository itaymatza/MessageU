/**
	Protocol response header file.
	Decode requests and encode responses by the protocol specification.
	@file protocol.h
	@author Itay Matza
	@version 2.0
*/
#ifndef __PROTOCOL_RESPNSE_H__
#define __PROTOCOL_RESPNSE_H__

#include <fstream>
#include <iostream>
#include <boost/asio.hpp>



// Supported response status codes.
enum ResponseCode : uint16_t {
	REGISTERED_SUCCESSFULLY = 1000,
	CLIENTS_LIST = 1001,
	PUBLIC_KEY = 1002,
	MESSAGE_SENT = 1003,
	PULL_WAITING_MASSAGES = 1004,
	GENERAL_ERROR = 9000
};

// Basic request header struct.
#pragma pack(push, 1)
struct RequestHeader
{
	uint32_t uid;
	uint8_t version;
	uint8_t op;
	uint16_t name_len;
};
#pragma pack(pop)


// Basic response header struct.
#pragma pack(push, 1)
struct ResponseHeader
{
	uint8_t version;
	uint16_t status;
};
#pragma pack(pop)


#endif /* __PROTOCOL_RESPNSE_H__ */