/**
	Protocol request header file.
	Decode requests and encode responses by the protocol specification.
	@file protocol_request.h
	@author Itay Matza
	@version 2.0
*/
#ifndef __PROTOCOL_REQUEST_H__
#define __PROTOCOL_REQUEST_H__

#include <fstream>
#include <iostream>
#include <boost/asio.hpp>

constexpr int CHUNK_SIZE = 1024;
constexpr int UID_LEN = 16;
constexpr int USERNAME_LEN = 255;
constexpr int PUBKEY_LEN = 160;
constexpr int CLIENT_VERSION = 2;


// Supported request operation codes.
enum RequestCode : uint8_t {
	REGISTER_REQUEST = 100,
	CLIENTS_LIST_REQUEST = 101,
	PUBLIC_KEY_REQUEST = 102,
	PUSH_MESSAGE_REQUEST = 103,
	PULL_MESSAGES_REQUEST = 104
};

// Request header struct.
#pragma pack(push, 1)
struct RequestHeader
{
	uint8_t uid[UID_LEN];
	uint8_t version;
	uint8_t code;
	uint32_t payoad_size;
};
#pragma pack(pop)

// Register request payload struct.
#pragma pack(push, 1)
struct RegisterRequestPayload
{
	uint8_t name[USERNAME_LEN];
	uint8_t public_key[PUBKEY_LEN];
};
#pragma pack(pop)

// Register request struct.
#pragma pack(push, 1)
struct RegisterRequest
{
	RequestHeader header;
	RegisterRequestPayload payload;
};
#pragma pack(pop)

// Clients list request struct.
#pragma pack(push, 1)
struct ClientsListRequest
{
	RequestHeader header;
};
#pragma pack(pop)

// Public key request payload struct.
#pragma pack(push, 1)
struct PublicKeyRequestPayload
{
	uint8_t uid[UID_LEN];
};
#pragma pack(pop)

// Public key request struct.
#pragma pack(push, 1)
struct PublicKeyRequest
{
	RequestHeader header;
	PublicKeyRequestPayload payload;
};
#pragma pack(pop)

// Pull messages request struct.
#pragma pack(push, 1)
struct PullMessagesRequest
{
	RequestHeader header;
};
#pragma pack(pop)

// Send message request payload struct.
#pragma pack(push, 1)
struct PushMessageRequestPayload
{
	uint8_t uid[UID_LEN];
	uint8_t message_type;
	uint32_t message_size;
};
#pragma pack(pop)

// Send message request struct.
#pragma pack(push, 1)
struct PushMessageRequest
{
	RequestHeader header;
	PushMessageRequestPayload payload;
};
#pragma pack(pop)

RegisterRequest* encodeRegisterRequest(std::string username, uint8_t public_key[]);
ClientsListRequest* encodeClientsListRequest(uint8_t uid[UID_LEN]);
PublicKeyRequest* encodePublicKeyRequest(uint8_t uid[UID_LEN], uint8_t other_uid[UID_LEN]);
PullMessagesRequest* encodePullMessagesRequest(uint8_t uid[UID_LEN]);
PushMessageRequest* encodePushMessageRequest(uint8_t uid[UID_LEN], uint8_t other_uid[UID_LEN], uint8_t message_type, size_t message_size);
void writeToServer(boost::asio::ip::tcp::socket& sock, uint8_t* request, unsigned long request_length);

#endif /* __PROTOCOL_REQUEST_H__ */