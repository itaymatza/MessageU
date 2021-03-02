/**
	MessageU-Client.
	Allows the user to send a message to another user who is registered on the MessageU-Server.

	@file messageu_client.cpp
	@author Itay Matza
	@version 2.0
*/
#include <iostream>
#include "client.h"
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
	string* ip = new string();
	string* port = new string();
	string* clien_name = new string();
	uint8_t uid[16];
	vector<Client*>* clients = new vector<Client*>();

	getServerInfo(ip, port, &status);
	getClientInfo(clien_name, uid, &status);

	boost::asio::io_context io_context;
	tcp::socket sock(io_context);
	tcp::resolver resolver(io_context);
	boost::asio::connect(sock, resolver.resolve(*ip, *port));

	bool another_request = true;
	while (another_request) {
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
			cout << "Selected option 1 - register" << endl;
			if (isFileExist("me.info")) {
				cout << "me.info file is already exists." << "\n" << endl;
			}
			else {
				string username;
				RegisterRequest* request;
				RegisterResponse* response;

				while (username.empty()) {
					cout << "Please enter new user name: ";
					getline(cin, username);
				}
				request = encodeRegisterRequest(username);
				writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(RegisterRequest));
				response = readServerRegisterResponse(sock);
				writeMeInfoFile(username, response->payload.uid, &status);
				cout << "Client registered successfully." << "\n" << endl;
				delete request;
				delete response;
			}
		}
			break;
		case 2:
		{
			cout << "Selected option 2 - clients list" << endl;
			ClientsListRequest* request;
			ClientsListResponse* response;

			request = encodeClientsListRequest(uid);
			writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(ClientsListRequest));
			cout << "MessageU - Clients list:" << endl;
			cout << "--------------------" << endl;
			response = readServerClientsListResponse(sock, clients);
			cout << "--------------------" << endl;
			cout << "End of Clients list." << "\n" << endl;
			delete request;
			delete response;
		}
			break;
		case 3:
		{
			string client_name;
			cout << "Selected option 3 - request public key" << endl;
			cout << "Please enter a client name to get a public key of: ";
			getline(cin, client_name);
			Client* wanted_client;
			bool client_in_memory = false;

			for (Client* client : *clients)
			{
				if (client_name == client->name)
				{
					wanted_client = client;
					PublicKeyRequest* request;
					PublicKeyResponse* response;

					request = encodePublicKeyRequest(uid, wanted_client->uid);
					writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PublicKeyRequest));
					response = readServerPublicKeyResponse(sock, client);
					delete request;
					delete response;
					client_in_memory = true;
					break;
				}
			}
			if (!client_in_memory)
			{
				cout << "Client name is not in memory, try to get clients list and try again." << "\n" << endl;
			}
		}
			break;
		case 4:
		{
			cout << "Selected option 4 - waiting messages" << endl;
			PullMessagesRequest* request;
			PullMessagesResponse* response;

			request = encodePullMessagesRequest(uid);
			writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(PullMessagesRequest));
			cout << "Pulling waiting messages:" << endl;
			cout << "-------------------------" << endl;
			response = readServerPullMessagesResponse(sock, clients);
			cout << "----------------" << endl;
			cout << "End of messages." << "\n" << endl;
			delete request;
			delete response;
		}
			break;
		case 5:
		{
			string client_name;
			cout << "Selected option 5 - send text message" << endl;
			cout << "Please enter a client name to send a message to: ";
			getline(cin, client_name);
			Client* wanted_client;
			bool client_in_memory = false;

			for (Client* client : *clients)
			{
				if (client_name == client->name)
				{
					string message;
					cout << "Please enter a message:" << endl;
					getline(cin, message);

					wanted_client = client;
					SendTextMessageRequest* request;
					SendTextMessageResponse* response;

					request = encodeSendTextMessageRequest(uid, wanted_client->uid, message.length());
					writeToServer(sock, reinterpret_cast<uint8_t*>(request), sizeof(SendTextMessageRequest));
					boost::asio::write(sock, boost::asio::buffer(message, message.length()));
					response = readServerSendTextMessageResponse(sock);
					delete request;
					delete response;
					client_in_memory = true;
					break;
				}
			}
			if (!client_in_memory)
			{
				cout << "Client name is not in memory, try to get clients list and try again." << "\n" << endl;
			}
		}
			break;
		case 50:
			cout << "Option 50" << endl;
			break;
		case 51:
			cout << "Option 51" << endl;
			break;
		case 52:
			cout << "Option 52" << endl;
			break;
		case 0:
			another_request = false;
			break;
		default:
			cout << "Invalid input. \nEnter again - ";
		}

		//if (system("CLS")) system("clear");
	}
	delete ip;
	delete port;
}