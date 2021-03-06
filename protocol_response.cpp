/**
	Decode responses by the protocol specification.
	@file protocol.cpp
	@author Itay Matza
	@version 1.0 01/03/21
*/

#include "protocol_response.h"
#include "data_helper.h"

using namespace std;
using boost::asio::ip::tcp;

constexpr int CHUNK_SIZE = 1024;
string ERROR_MSG("server responded with an error.\n");


// Reads response header from socket, return false if server responds with general error
bool readResponseHeader(tcp::socket& sock, ResponseHeader* response_header) {
	boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(response_header), sizeof(ResponseHeader)));
	if (response_header->code == ResponseCode::GENERAL_ERROR)
		return false;
	return true;
}


RegisterResponse* readServerRegisterResponse(tcp::socket& sock) {
	RegisterResponse* response = new RegisterResponse;
	if (readResponseHeader(sock, &response->header))
		boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(RegisterResponsePayload)));
	return response;
}

ClientsListResponse* readServerClientsListResponse(tcp::socket& sock, vector<Client*>* clients) {
	ClientsListResponse* response = new ClientsListResponse;
	if (readResponseHeader(sock, &response->header) == false)
		return response;

	size_t list_length = response->header.payoad_size / sizeof(ClientsListResponsePayload);
	while (0 < list_length)
	{
		boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(ClientsListResponsePayload)));
		Client* client = new Client();
		string str(reinterpret_cast<char*>(&response->payload.client_name));
		client->name = str;
		memcpy(client->uid, response->payload.uid, sizeof(client->uid));
		clients->push_back(client);
		cout << client->name << endl;
		list_length -= 1;
	}
	return response;
}


PublicKeyResponse* readServerPublicKeyResponse(boost::asio::ip::tcp::socket& sock, Client* client) {
	PublicKeyResponse* response = new PublicKeyResponse;
	if (readResponseHeader(sock, &response->header)) {
		boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(PublicKeyResponsePayload)));
		memcpy(client->public_key, response->payload.public_key, sizeof(client->public_key));
	}
	else {
		cout << ERROR_MSG << endl;
	}
	return response;
}


PullMessagesResponse* readServerPullMessagesResponse(boost::asio::ip::tcp::socket& sock, std::vector<Client*>* clients, string* private_key) {
	PullMessagesResponse* response = new PullMessagesResponse;
	if (readResponseHeader(sock, &response->header) == false)
		return response;

	size_t list_length = response->header.payoad_size;
	while (0 < list_length)
	{
		boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(PullMessagesResponsePayload)));
		list_length -= sizeof(PullMessagesResponsePayload);

		cout << "From:\t";
		bool client_in_memory = false;
		Client* wanted_client = new Client();
		for (Client* client : *clients)
		{
			if (memcmp(client->uid, response->payload.uid, sizeof(client->uid)) == 0)
			{
				cout << client->name << endl;
				wanted_client = client;
				client_in_memory = true;
				break;
			}
		}
		if (!client_in_memory)
		{
			cout << "Error to fetch user name - the user name is not in memory" << endl;
		}
		cout << "Content:" << endl;
		if (response->payload.message_type == MessageType::REQUEST_FOR_SYMMETRIC_KEY)
		{
			cout << "Request for symmetric key." << endl;
		}
		else if (response->payload.message_type == MessageType::SYMMETRIC_KEY)
		{
			vector<char> message(response->payload.message_size);
			boost::asio::read(sock, boost::asio::buffer(message, response->payload.message_size));
			std::string ciphertext(message.begin(), message.end());

			cout << "Symmetric key received." << endl;
			string decrypted_symmetric_key = decryptRsaString(*private_key, ciphertext);
			copy(decrypted_symmetric_key.begin(), decrypted_symmetric_key.begin() + AES_KEYSIZE, std::begin(wanted_client->symmetric_key));
		}
		else if (response->payload.message_type == MessageType::TEXT_MESSAGE)
		{
			vector<char> message(response->payload.message_size);
			boost::asio::read(sock, boost::asio::buffer(message, response->payload.message_size));
			std::string ciphertext(message.begin(), message.end());

			string decrypted = decryptAesString(wanted_client->symmetric_key, ciphertext);
			cout << decrypted << endl;
		}
		else if (response->payload.message_type == MessageType::SEND_FILE)
		{
			if (!createTmpDirectory())
			{
				break;
			}
			string filename = writeReceivedPayloadToFile(sock, response->payload.message_size);
			string decrypted_file = decryptAesFile(wanted_client->symmetric_key, filename);
			cout << "File saved to - " << decrypted_file << endl;
			deleteFile(filename);
		}	
		list_length -= response->payload.message_size;
		cout << "-----<EOM>-----\n" << endl;
	}
	return response;
}

PushMessageResponse* readServerPushMessageResponse(boost::asio::ip::tcp::socket& sock) {
	PushMessageResponse* response = new PushMessageResponse;
	if (readResponseHeader(sock, &response->header))
		boost::asio::read(sock, boost::asio::buffer(reinterpret_cast<uint8_t*>(&response->payload), sizeof(PushMessageResponsePayload)));
	else
		cout << ERROR_MSG << endl;
	return response;
}

bool isServerRespondedWithError(uint16_t code)
{
	if (code == ResponseCode::GENERAL_ERROR)
	{
		cout << ERROR_MSG << endl;
		return true;
	}
	return false;
}