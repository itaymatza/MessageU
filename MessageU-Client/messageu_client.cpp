/**
	MessageU-Client.
	Allows the user to send a message to another user who is registered on the MessageU-Server.

	@file messageu_client.cpp
	@author Itay Matza
	@version 2.0
*/
#include <iostream>
#include "client.h"
#include "crypto.h"
#include "data_helper.h"
#include "protocol_request.h"
#include "protocol_response.h"

using namespace std;
using boost::asio::ip::tcp;
string welcome_msg("MessageU client at your service.");
string client_info_file("me.info");
string server_info_file("server.info");


// MessageU-Client Main Function.
int main() {
	uint8_t uid[UID_LEN];
	string* server_ip = new string();
	string* server_port = new string();
	string* client_name = new string();
	string* private_key = new string();
	vector<Client*>* clients_list = new vector<Client*>();

	getServerInfoFromFile(server_ip, server_port, server_info_file);
	getClientInfoFromFile(client_name, uid, private_key, client_info_file);
	boost::asio::io_context io_context;
	tcp::socket sock(io_context);
	tcp::resolver resolver(io_context);
	try {
		boost::asio::connect(sock, resolver.resolve(*server_ip, *server_port));
	}
	catch (...) {
		cout << "Error: Unable to connect to server." << endl;;
		exit(-1);
	}

	bool proceed_to_another_request = true;
	while (proceed_to_another_request) {
		string input;
		int option;

		cout << welcome_msg << endl << "\n";
		cout << "1) Register" << endl;
		cout << "2) Request for clients list" << endl;
		cout << "3) Request for public key" << endl;
		cout << "4) Request for waiting messages" << endl;
		cout << "5) Send a text message" << endl;
		cout << "50) Send a file" << endl;
		cout << "51) Send a request for symmetric key" << endl;
		cout << "52) Send your symmetric key" << endl;
		cout << "0) Exit client" << endl;
		getline(cin, input);
		try {
			option = stoi(input);
		}
		catch (exception& err)
		{
			cout << "Invalid input. \nEnter again - ";
			cin.clear();
			continue;
		}

		switch (option) {
		case 1:
		{
			cout << "Selected option 1 - Register" << endl;
			if (isFileExist(client_info_file)) {
				cout << client_info_file << " file is already exists." << "\n" << endl;
				break;
			}
			string username;
			while (username.empty()) {
				cout << "Please enter new user name: ";
				getline(cin, username);
			}

			uint8_t public_key[PUBKEY_LEN];
			genRsaKeyPair(public_key, client_info_file);
			RegisterRequest* request = encodeRegisterRequest(username, public_key);
			writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(RegisterRequest));

			RegisterResponse* response = readServerRegisterResponse(sock);
			if (isServerRespondedWithError(response->header.code)) {
				deleteFile(client_info_file);
			}
			else {
				writeMeInfoFile(username, response->payload.uid);
				cout << "Client registered successfully." << "\n" << endl;
				getClientInfoFromFile(client_name, uid, private_key, client_info_file);
			}
			delete request;
			delete response;
			break;
		}
		case 2:
		{
			cout << "Selected option 2 - Request for clients list" << endl;

			ClientsListRequest* request = encodeClientsListRequest(uid);
			writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(ClientsListRequest));
			cout << "MessageU - Clients list:" << endl;
			cout << "--------------------" << endl;
			ClientsListResponse* response = readServerClientsListResponse(sock, clients_list);
			if (isServerRespondedWithError(response->header.code) == false) {
				cout << "--------------------" << endl;
				cout << "End of Clients list." << "\n" << endl;
			}
			delete request;
			delete response;
			break;
		}
		case 3:
		{
			cout << "Selected option 3 - Request for public key" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(&wanted_client, clients_list))
			{
				PublicKeyRequest* request = encodePublicKeyRequest(uid, wanted_client->uid);
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PublicKeyRequest));
				readServerPublicKeyResponse(sock, wanted_client);
				delete request;
			}
			break;
		}
		case 4:
		{
			cout << "Selected option 4 - Request for waiting messages" << endl;

			PullMessagesRequest* request = encodePullMessagesRequest(uid);
			writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PullMessagesRequest));
			cout << "Pulling waiting messages:" << endl;
			cout << "-------------------------" << endl;
			PullMessagesResponse* response = readServerPullMessagesResponse(sock, clients_list, private_key);
			if (isServerRespondedWithError(response->header.code) == false) {
				cout << "-------------------------" << endl;
				cout << "End of messages." << "\n" << endl;
			}
			delete request;
			delete response;
			break;
		}
		case 5:
		{
			cout << "Selected option 5 - Send a text message" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(&wanted_client, clients_list))
			{
				string message;
				cout << "Please enter a message:" << endl;
				getline(cin, message);
				if (isDefaultAesKey(wanted_client->symmetric_key)) {
					cout << "Can't encrypt message." << endl;
					break;
				}
				try {
					string ciphertext = encryptAesString(wanted_client->symmetric_key, message);

					PushMessageRequest* request = encodePushMessageRequest(uid, wanted_client->uid, MessageType::TEXT_MESSAGE, ciphertext.length());
					writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PushMessageRequest));
					boost::asio::write(sock, boost::asio::buffer(ciphertext, ciphertext.size()));
					readServerPushMessageResponse(sock);
					delete request;
				}
				catch (...) {
					cout << "Can't encrypt message." << endl;
				}
			}
			break;
		}
		case 50:
		{
			cout << "Selected option 50 - Send a file" << endl;
			Client* wanted_client = new Client();
			if (getClientFromInput(&wanted_client, clients_list))
			{
				cout << "Please enter a file name: ";
				string file;
				getline(std::cin, file);
				if (!isFileExist(file))
					break;
				if (isDefaultAesKey(wanted_client->symmetric_key)) {
					cout << "Can't encrypt file." << endl;
					break;
				}
				try {
					string encrypted_file = encryptAesFile(wanted_client->symmetric_key, file);
					PushMessageRequest* request = encodePushMessageRequest(uid, wanted_client->uid, MessageType::SEND_FILE, getFileSize(encrypted_file));
					writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PushMessageRequest));
					writeRequestPayloadFromFile(sock, encrypted_file, getFileSize(encrypted_file));
					readServerPushMessageResponse(sock);
					deleteFile(encrypted_file);
					delete request;
				}
				catch (...) {
					cout << "Can't encrypt file." << endl;
				}
			}
			break;
		}
		case 51:
		{
			cout << "Selected option 51 - Send a request for symmetric key" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(&wanted_client, clients_list))
			{
				PushMessageRequest* request = encodePushMessageRequest(uid, wanted_client->uid, MessageType::REQUEST_FOR_SYMMETRIC_KEY, 0);
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PushMessageRequest));
				readServerPushMessageResponse(sock);
				delete request;
			}
			break;
		}
		case 52:
		{
			cout << "Selected option 52 - Send your symmetric key" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(&wanted_client, clients_list))
			{
				if (isDefaultRsaKey(wanted_client->public_key)) {
					cout << "Can't encrypt symmetric key." << endl;
					break;
				}
				try {
					genAesKey(wanted_client->symmetric_key);
					string encrypted_symmetric_key = encryptRsaString(wanted_client->public_key, reinterpret_cast<char*>(wanted_client->symmetric_key));

					PushMessageRequest* request = encodePushMessageRequest(uid, wanted_client->uid, MessageType::SYMMETRIC_KEY, encrypted_symmetric_key.size());
					writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PushMessageRequest));
					boost::asio::write(sock, boost::asio::buffer(encrypted_symmetric_key, encrypted_symmetric_key.size()));
					readServerPushMessageResponse(sock);
					delete request;
				}
				catch (...) {
					cout << "Can't encrypt symmetric key." << endl;
				}
			}
			break;
		}
		case 0:
			proceed_to_another_request = false;
			break;
		default:
			cout << "Invalid input. \nEnter again - ";
		}
	}
	deleteClientList(*clients_list);
	delete clients_list;
	delete server_ip;
	delete server_port;
	delete client_name;
	delete private_key;
}