#include "client.h"
#include <iostream>

bool getClientFromInput(Client** wanted_client, std::vector<Client*>* clients) {
	std::cout << "Please enter a client name: ";
	std::string client_name;
	std::getline(std::cin, client_name);

	for (Client* client : *clients)
	{
		if (client_name == client->name)
		{
			*wanted_client = client;
			return true;
		}
	}
	std::cout << "Client name is not in memory, try to get clients list and try again.\n" << std::endl;
	return false;
}


// Delete client if irrelevant data and determine if need to update 	 
bool needToAddToClientsList(Client* wanted_client, std::vector<Client*> clients) {
	for (auto it = clients.begin(); it != clients.end(); ) {
		bool same_name = (*it)->name.compare(wanted_client->name) == 0;
		bool same_uid = memcmp((*it)->uid, wanted_client->uid, sizeof(wanted_client->uid)) == 0;

		if (same_name && same_uid)
			return false;
		else if (same_name || same_uid) {
			delete* it;
			it = clients.erase(it);
		}
		else {
			++it;
		}
	}
	return true;
}