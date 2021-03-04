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
};

bool isClientExistsInClientsList(Client* wanted_client, std::vector<Client*>* clients);

#endif /* __CLIENT__ */