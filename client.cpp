#include "client.h"
#include <iostream>

bool getClientFromInput(Client* wanted_client, std::vector<Client*>* clients) {
	std::cout << "Please enter a client name: ";
	std::string client_name;
	std::getline(std::cin, client_name);

	for (Client* client : *clients)
	{
		if (client_name == client->name)
		{
			*wanted_client = *client;
			return true;
		}
	}
	return false;
}