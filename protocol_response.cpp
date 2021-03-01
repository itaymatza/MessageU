/**
	Decode responses by the protocol specification.
	@file protocol.cpp
	@author Itay Matza
	@version 1.0 01/03/21
*/
#include "protocol_response.h"
using namespace std;
using boost::asio::ip::tcp;
constexpr int CHUNK_SIZE = 1024;


RegisterResponse* readServerResponse(tcp::socket& sock) {
	RegisterResponse* response = new RegisterResponse;
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->header), sizeof(ResponseHeader)));
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(RegisterResponsePayload)));
	return response;
}