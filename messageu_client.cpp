/**
	MessageU-Client.
	Allows the user to send a message to another user who is registered on the MessageU-Server.

	@file messageu_client.cpp
	@author Itay Matza
	@version 2.0
*/
#include <iostream>
#include "data_helper.h"

using namespace std;
constexpr int SRV_VERSION = 2;
string welcome_msg("MessageU client at your service.");

// MessageU-Client Main Function.
int main() {
	string* ip = new string();
	string* port = new string();
	string* clien_name = new string();
	vector<unsigned char>* uid = new vector<unsigned char>();

	getServerInfo(ip, port);
	getClientInfo(clien_name, uid);
	cout << *clien_name;

	bool get_input = true;
	while (get_input) {
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
		cin >> option;

		if (!cin) {
			cout << "Invalid input. \nEnter again - ";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		switch (option) {
		case 1:
			cout << "Option 1" << endl;
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
			get_input = false;
			break;
		default:
			cout << "Invalid input. \nEnter again - ";
		}

		//if (system("CLS")) system("clear");
	}

	delete ip;
	delete port;
}

