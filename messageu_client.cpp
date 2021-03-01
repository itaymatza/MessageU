/**
	MessageU-Client.
	Allows the user to send a message to another user who is registered on the MessageU-Server.

	@file messageu_client.cpp
	@author Itay Matza
	@version 2.0
*/
#include <iostream>
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
	vector<unsigned char>* uid = new vector<unsigned char>();

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
			//cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		switch (option) {
		case 1:
			cout << "Option 1" << endl;
			if (isFileExist("me.info")) {
				cout << "me.info file is already exists." << endl << "\n";
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
				response = readServerResponse(sock);
				writeMeInfoFile(username, response->payload.uid, &status);
				delete request;
			}
			break;
		case 2:
			cout << "Option 2" << endl;
			break;
		case 3:
			cout << "Option 3" << endl;
			break;
		case 4:
			cout << "Option 4" << endl;
			break;
		case 5:
			cout << "Option 5" << endl;
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