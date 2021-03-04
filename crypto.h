/**
	Crypto.
	@file crypto.h
	@author Itay Matza
	@version 1.0 01/03/21
*/

#ifndef __CRYPTO__
#define __CRYPTO__

void genRsaKeyPair(uint8_t public_key[]);
std::string encryptString(uint8_t public_key[], std::string plain_text);
std::string decryptString(std::string private_key, std::string ciphertext);


#endif /* __CRYPTO__ */