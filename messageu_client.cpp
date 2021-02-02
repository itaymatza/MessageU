/**
	MessageU-Client.
	Allows the user to send a message to another user who is registered on the MessageU-Server.

	@file messageu_client.cpp
	@author Itay Matza
	@version 2.0
*/
#include <iostream>
#include "data_helper.h"
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;
constexpr int SRV_VERSION = 2;

// MessageU-Client Main Function.
int main() {
	string* ip = new string();
	string* port = new string();

	getServerInfo(ip, port);
	cout << *ip << " " << *port;
	boost::asio::io_context io_context;

	//createDirectory(SRV_DIR);
	//try
	//{
	//	server(io_context, port);
	//}
	//catch (std::exception& e)
	//{
	//	cerr << "Exception: " << e.what() << "\n";
	//	exit(-1);
	//}
	delete ip;
	delete port;
}

