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
	int option;
	string* ip = new string();
	string* port = new string();
	string* clien_name = new string();
	vector<unsigned char>* uid = new vector<unsigned char>();

	getServerInfo(ip, port);
	getClientInfo(clien_name, uid);
	cout << *clien_name;

	do
	{
		cout << welcome_msg << "\n\n";
		cout << "1) Register" << "\n";
		cout << "2) Request for clients list" << "\n";
		cout << "3) Request for public key" << "\n";
		cout << "4) Request for waiting messages" << "\n";
		cout << "5) Send a text message" << "\n";
		cout << "50) Send a file" << "\n";
		cout << "51) Send a request for symmetric key" << "\n";
		cout << "52) Send your symmetric key" << "\n";
		cout << "0) Exit client" << "\n";
		cin >> option;

	} while (option != 0);


	delete ip;
	delete port;
}

