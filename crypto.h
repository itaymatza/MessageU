/**
	Crypto - methods for encryption/decryption.
	@file crypto.h
	@author Itay Matza
	@version 1.0 01/03/21
*/

#ifndef __CRYPTO__
#define __CRYPTO__

static const size_t AES_KEYSIZE = 16;
static const size_t RSA_PUBLIC_KEYSIZE = 160;

// RSA
void genRsaKeyPair(uint8_t public_key[RSA_PUBLIC_KEYSIZE]);
std::string encryptRsaString(uint8_t public_key[RSA_PUBLIC_KEYSIZE], std::string plain_text);
std::string decryptRsaString(std::string private_key, std::string ciphertext);

// AES
void genAesKey(uint8_t key[AES_KEYSIZE]);
std::string encryptAesString(uint8_t key[AES_KEYSIZE], std::string plaintext);
std::string decryptAesString(uint8_t key[AES_KEYSIZE], std::string ciphertext);
std::string encryptAesFile(uint8_t key[AES_KEYSIZE], std::string filename_in);
std::string decryptAesFile(uint8_t key[AES_KEYSIZE], std::string filename_in);
#endif /* __CRYPTO__ */