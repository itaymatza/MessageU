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
string error_msg("server responded with an error.");


// MessageU-Client Main Function.
int main() {
	Status status = Status::proper;
	string* server_ip = new string();
	string* server_port = new string();
	string* client_name = new string();
	string* private_key = new string();
	uint8_t uid[UID_LEN];
	vector<Client*>* clients_list = new vector<Client*>();

	getServerInfoFromFile(server_ip, server_port, &status);
	getClientInfoFromFile(client_name, uid, private_key, &status);

	boost::asio::io_context io_context;
	tcp::socket sock(io_context);
	tcp::resolver resolver(io_context);
	boost::asio::connect(sock, resolver.resolve(*server_ip, *server_port));

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
			if (isFileExist("me.info")) {
				cout << "me.info file is already exists." << "\n" << endl;
			}
			else {
				string username;
				while (username.empty()) {
					cout << "Please enter new user name: ";
					getline(cin, username);
				}

				uint8_t public_key[PUBKEY_LEN];
				GenRsaKeyPair(public_key);
				RegisterRequest* request = encodeRegisterRequest(username, public_key);
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(RegisterRequest));
				RegisterResponse* response = readServerRegisterResponse(sock);
				writeMeInfoFile(username, response->payload.uid, &status);
				cout << "Client registered successfully." << "\n" << endl;
				getClientInfoFromFile(client_name, uid, private_key, &status);

				delete request;
				delete response;
			}
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
			cout << "--------------------" << endl;
			cout << "End of Clients list." << "\n" << endl;

			delete request;
			delete response;
			break;
		}
		case 3:
		{
			cout << "Selected option 3 - Request for public key" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(wanted_client, clients_list))
			{
				PublicKeyRequest* request = encodePublicKeyRequest(uid, wanted_client->uid);
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PublicKeyRequest));
				PublicKeyResponse* response = readServerPublicKeyResponse(sock, wanted_client);

				delete request;
				delete response;
			}
			break;
		}
		case 4:
		{
			cout << "Selected option 4 - Request for waiting messages" << endl;

			PullMessagesRequest*  request = encodePullMessagesRequest(uid);
			writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PullMessagesRequest));
			cout << "Pulling waiting messages:" << endl;
			cout << "-------------------------" << endl;
			PullMessagesResponse* response = readServerPullMessagesResponse(sock, clients_list);
			cout << "----------------" << endl;
			cout << "End of messages." << "\n" << endl;

			delete request;
			delete response;
			break;
		}
		case 5:
		{
			cout << "Selected option 5 - Send a text message" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(wanted_client, clients_list))
			{
				string message;
				cout << "Please enter a message:" << endl;
				getline(cin, message);

				PushMessageRequest* request = encodePushTextMessageRequest(uid, wanted_client->uid, message.length());
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PushMessageRequest));
				boost::asio::write(sock, boost::asio::buffer(message, message.length()));
				PushMessageResponse* response = readServerPushMessageResponse(sock);

				delete request;
				delete response;
			}
			break;
		}
		case 50:
			cout << "Option 50" << endl;
			break;
		case 51:
		{
			cout << "Selected option 51 - Send a request for symmetric key" << endl;
			Client* wanted_client = new Client();

			if (getClientFromInput(wanted_client, clients_list))
			{
				PushMessageRequest* request = encodePushReqKeyMessageRequest(uid, wanted_client->uid);
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PushMessageRequest));
				PushMessageResponse* response = readServerPushMessageResponse(sock);

				delete request;
				delete response;
			}
			break;
		}
		case 52:
			cout << "Option 52" << endl;
			break;
		case 0:
			proceed_to_another_request = false;
			break;
		default:
			cout << "Invalid input. \nEnter again - ";
		}

		//if (system("CLS")) system("clear");
	}
	delete server_ip;
	delete server_port;
}