/**
	Client.
	@file client.h
	@author Itay Matza
	@version 1.0 01/03/21
*/
#include <vector>
#include <string>

#ifndef __CLIENT__
#define __CLIENT__

class Client {
public:
	std::string name;
	uint8_t uid[16];
	uint8_t public_key[160];
	uint8_t symmetric_key[16];
};

bool getClientFromInput(Client** wanted_client, std::vector<Client*>* clients);
bool needToAddToClientsList(Client* wanted_client, std::vector<Client*> clients);
void deleteClientList(std::vector<Client*> clients);
#endif /* __CLIENT__ */