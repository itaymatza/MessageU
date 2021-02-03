/**
	MessageU-Client.
	Allows the user to send a message to another user who is registered on the MessageU-Server.

	@file messageu_client.cpp
	@author Itay Matza
	@version 2.0
*/
#include <iostream>
#include <vector>
#include "data_helper.h"

using namespace std;
constexpr int SRV_VERSION = 2;

// MessageU-Client Main Function.
int main() {
	string* ip = new string();
	string* port = new string();
	string* clien_name = new string();
	std::vector<unsigned char>* uid;

	getServerInfo(ip, port);
	getClienInfo(clien_name, uid);


	delete ip;
	delete port;
}

