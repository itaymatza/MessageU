/**
	Protocol request header file.
	Decode requests and encode responses by the protocol specification.
	@file protocol.h
	@author Itay Matza
	@version 2.0
*/
#ifndef __PROTOCOL_REQUEST_H__
#define __PROTOCOL_REQUEST_H__

#include <fstream>
#include <iostream>
#include <boost/asio.hpp>


// Supported request operation codes.
enum RequestCode : uint8_t {
	REGISTER_REQUEST = 100,
	CLIENTS_LIST_REQUEST = 101,
	PUBLIC_KEY_REQUEST = 102,
	SEND_MESSAGE_REQUEST = 103,
	PULL_MESSAGE_REQUEST = 104
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

// Basic request payload struct.
#pragma pack(push, 1)
struct RequestPayload
{
	uint32_t size;
};
#pragma pack(pop)


#endif /* __PROTOCOL_REQUEST_H__ */