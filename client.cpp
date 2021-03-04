#include "client.h"


bool isClientExistsInClientsList(std::string client_name, Client* wanted_client, std::vector<Client*>* clients) {
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