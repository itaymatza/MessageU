/**
	Encode requests by the protocol specification.
	@file protocol.cpp
	@author Itay Matza
	@version 1.0 01/03/21
*/
#include "protocol_request.h"
using namespace std;
using boost::asio::ip::tcp;

// Writes to server. 
void writeToServer(tcp::socket& sock, uint8_t* request, unsigned long request_length) {
	size_t total_sent = 0;
	while (request_length > 0) {
		size_t size = min(static_cast<unsigned long>(CHUNK_SIZE), request_length);
		size_t sent = boost::asio::write(sock, boost::asio::buffer(request + total_sent, size));
		request_length -= sent;
	}
}

// Encode register request by the protocol specification.
RegisterRequest* encodeRegisterRequest(string username) {
	RegisterRequest* request = new RegisterRequest;
	request->header.version = CLIENT_VERSION;
	request->header.code = RequestCode::REGISTER_REQUEST;
	request->header.payoad_size = sizeof(RegisterRequestPayload);
	size_t size = min(username.size(), (size_t) USERNAME_LEN - 1);
	copy(username.begin(), username.begin() + size, std::begin(request->payload.name));
	request->payload.name[size] = '\0';
	return request;
}

// Encode client list request by the protocol specification.
ClientsListRequest* encodeClientsListRequest(uint8_t uid[16]) {
	ClientsListRequest* request = new ClientsListRequest;
	memcpy(request->header.uid, uid, 16);
	request->header.version = CLIENT_VERSION;
	request->header.code = RequestCode::CLIENTS_LIST_REQUEST;
	request->header.payoad_size = 0;
	return request;
}

// Encode public key request by the protocol specification.
PublicKeyRequest* encodePublicKeyRequest(uint8_t uid[16], uint8_t other_uid[16]) {
	PublicKeyRequest* request = new PublicKeyRequest;
	memcpy(request->header.uid, uid, 16);
	request->header.version = CLIENT_VERSION;
	request->header.code = RequestCode::PUBLIC_KEY_REQUEST;
	request->header.payoad_size = sizeof(PublicKeyRequestPayload);
	memcpy(request->payload.uid, other_uid, 16);
	return request;
}

// Encode pull messages request by the protocol specification.
PullMessagesRequest* encodePullMessagesRequest(uint8_t uid[16]){
	PullMessagesRequest* request = new PullMessagesRequest;
	memcpy(request->header.uid, uid, 16);
	request->header.version = CLIENT_VERSION;
	request->header.code = RequestCode::PULL_MESSAGES_REQUEST;
	request->header.payoad_size = 0;
	return request;
}

// Encode public key request by the protocol specification.
SendTextMessageRequest* encodeSendTextMessageRequest(uint8_t uid[16], uint8_t other_uid[16], size_t message_size) {
	SendTextMessageRequest* request = new SendTextMessageRequest;
	memcpy(request->header.uid, uid, 16);
	request->header.version = CLIENT_VERSION;
	request->header.code = RequestCode::PUSH_MESSAGE_REQUEST;
	request->header.payoad_size = sizeof(SendTextMessageRequestPayload) + message_size;
	memcpy(request->payload.uid, other_uid, 16);
	request->payload.message_type = MessageType::TEXT_MESSAGE;
	request->payload.message_size = message_size;
	return request;
}