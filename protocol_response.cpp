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


RegisterResponse* readServerRegisterResponse(tcp::socket& sock) {
	RegisterResponse* response = new RegisterResponse;
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->header), sizeof(ResponseHeader)));
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(RegisterResponsePayload)));
	return response;
}

ClientsListResponse* readServerClientsListResponse(tcp::socket& sock, vector<Client*>* clients) {
	ClientsListResponse* response = new ClientsListResponse;
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->header), sizeof(ResponseHeader)));
	size_t list_length = response->header.payoad_size / sizeof(ClientsListResponsePayload);
	while (0 < list_length)
	{
		boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(ClientsListResponsePayload)));
		Client* client = new Client();
		string str(reinterpret_cast<char*>(&response->payload.client_name));
		client->name = str;
		memcpy(client->uid, response->payload.uid, sizeof(client->uid));
		memset(client->public_key, 0, sizeof(client->public_key));
		clients->push_back(client);
		cout << client->name << endl;
		list_length -= 1;
	}
	return response;
}


PublicKeyResponse* readPublicKeyResponse(boost::asio::ip::tcp::socket& sock, Client* client) {
	PublicKeyResponse* response = new PublicKeyResponse;
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->header), sizeof(ResponseHeader)));
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(PublicKeyResponsePayload)));
	memcpy(client->public_key, response->payload.public_key, sizeof(client->public_key));
	return response;
}