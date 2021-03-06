/**
	Protocol response header file.
	Decode requests and encode responses by the protocol specification.
	@file protocol_response.h
	@author Itay Matza
	@version 2.0
*/
#ifndef __PROTOCOL_RESPNSE_H__
#define __PROTOCOL_RESPNSE_H__

#include <fstream>
#include <iostream>
#include <boost/asio.hpp>
#include "message.h"
#include "client.h"
#include "crypto.h"

// Supported response status codes.
enum ResponseCode : uint16_t {
	REGISTERED_SUCCESSFULLY = 1000,
	CLIENTS_LIST = 1001,
	PUBLIC_KEY = 1002,
	PUSH_MESSAGE = 1003,
	PULL_MESSAGES = 1004,
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

// Public key response payload struct.
#pragma pack(push, 1)
struct PublicKeyResponsePayload
{
	uint8_t uid[16];
	uint8_t public_key[160];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PublicKeyResponse
{
	ResponseHeader header;
	PublicKeyResponsePayload payload;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PullMessagesResponsePayload
{
	uint8_t uid[16];
	uint32_t message_id;
	uint8_t message_type;
	uint32_t message_size;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PullMessagesResponse
{
	ResponseHeader header;
	PullMessagesResponsePayload payload;
};
#pragma pack(pop)

// Send text message response payload struct.
#pragma pack(push, 1)
struct PushMessageResponsePayload
{
	uint8_t uid[16];
	uint32_t message_id;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PushMessageResponse
{
	ResponseHeader header;
	PushMessageResponsePayload payload;
};
#pragma pack(pop)

RegisterResponse* readServerRegisterResponse(boost::asio::ip::tcp::socket& sock);
ClientsListResponse* readServerClientsListResponse(boost::asio::ip::tcp::socket& sock, std::vector<Client*>* clients);
PublicKeyResponse* readServerPublicKeyResponse(boost::asio::ip::tcp::socket& sock, Client* client);
PullMessagesResponse* readServerPullMessagesResponse(boost::asio::ip::tcp::socket& sock, std::vector<Client*>* clients, std::string* private_key);
PushMessageResponse* readServerPushMessageResponse(boost::asio::ip::tcp::socket& sock);
bool isServerRespondedWithError(uint16_t code);

#endif /* __PROTOCOL_RESPNSE_H__ */