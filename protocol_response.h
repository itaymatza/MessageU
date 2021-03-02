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
#include "client.h"


// Supported response status codes.
enum ResponseCode : uint16_t {
	REGISTERED_SUCCESSFULLY = 1000,
	CLIENTS_LIST = 1001,
	PUBLIC_KEY = 1002,
	MESSAGE_SENT = 1003,
	PULL_WAITING_MASSAGES = 1004,
	GENERAL_ERROR = 9000
};

// Response header struct.
#pragma pack(push, 1)
struct ResponseHeader
{
	uint8_t version;
	uint16_t code;
	uint32_t payoad_size;
};
#pragma pack(pop)

// Register response payload struct.
#pragma pack(push, 1)
struct RegisterResponsePayload
{
	uint8_t uid[16];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RegisterResponse 
{
	ResponseHeader header;
	RegisterResponsePayload payload;
};
#pragma pack(pop)

// Client list response payload struct.
#pragma pack(push, 1)
struct ClientsListResponsePayload
{
	uint8_t uid[16];
	uint8_t client_name[255];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ClientsListResponse
{
	ResponseHeader header;
	ClientsListResponsePayload payload;
};
#pragma pack(pop)

RegisterResponse* readServerRegisterResponse(boost::asio::ip::tcp::socket& sock);
ClientsListResponse* readServerClientsListResponse(boost::asio::ip::tcp::socket& sock, std::vector<Client*>* clients);

#endif /* __PROTOCOL_RESPNSE_H__ */