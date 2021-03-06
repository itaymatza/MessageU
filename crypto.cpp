/**
	Crypto - methods for encryption/decryption.
	@file crypto.cpp
	@author Itay Matza
	@version 1.0
*/

#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>
#include <string>
#include <fstream>
#include "crypto.h"
#include "data_helper.h"
#include <modes.h>
#include <aes.h>
#include <filters.h>
#include <iostream>
#include <string>
#include <immintrin.h>	// _rdrand32_step



// Generate RSA key pair (private and public), saves the private key to disk in base64 format and returns the public key
void genRsaKeyPair(uint8_t public_key[RSA_PUBLIC_KEYSIZE], std::string file)
{
	// private key generation
	CryptoPP::AutoSeededRandomPool rng;
	CryptoPP::InvertibleRSAFunction privkey;
	privkey.Initialize(rng, 1024);
	CryptoPP::Base64Encoder privkeysink(new CryptoPP::FileSink(file.c_str()));
	privkey.DEREncode(privkeysink);
	privkeysink.MessageEnd();

	// public key generation
	CryptoPP::RSAFunction pubkey(privkey);
	std::string public_key_string;
	CryptoPP::Base64Encoder pubkeysink(new CryptoPP::StringSink(public_key_string));
	pubkey.DEREncode(pubkeysink);
	pubkeysink.MessageEnd();

	// Save the key to an unsigned char buffer
	CryptoPP::ByteQueue bytes;
	CryptoPP::StringSource pkey(public_key_string, true, new CryptoPP::Base64Decoder);
	pkey.TransferTo(bytes);
	bytes.MessageEnd();
	CryptoPP::RSA::PublicKey pubKey;
	pubKey.Load(bytes);

	CryptoPP::ArraySink as(public_key, RSA_PUBLIC_KEYSIZE);
	pubKey.Save(as);
}


// Encrypt plain text string using RSA public key
std::string encryptRsaString(uint8_t public_key[RSA_PUBLIC_KEYSIZE], std::string plaintext)
{
	CryptoPP::AutoSeededRandomPool rng;

	// from uint8_t buffer to public key
	CryptoPP::ArraySource as(public_key, RSA_PUBLIC_KEYSIZE, true);
	CryptoPP::RSA::PublicKey pubKey;
	pubKey.Load(as);

	// encrypt plain text
	std::string ciphertext;
	CryptoPP::RSAES_OAEP_SHA_Encryptor e(pubKey);
	CryptoPP::StringSource ss(plaintext, true, new CryptoPP::PK_EncryptorFilter(rng, e, new CryptoPP::StringSink(ciphertext)));

	return ciphertext;
}


// Decrypt ciphertext string using RSA private key
std::string decryptRsaString(std::string private_key, std::string ciphertext)
{
	CryptoPP::AutoSeededRandomPool rng;

	// read private key from string
	CryptoPP::ByteQueue bytes;
	CryptoPP::StringSource keystring(private_key, true, new CryptoPP::Base64Decoder);
	keystring.TransferTo(bytes);
	bytes.MessageEnd();
	CryptoPP::RSA::PrivateKey privateKey;
	privateKey.Load(bytes);

	// decrypt ciphertext
	std::string decrypted;
	CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);
	CryptoPP::StringSource ss(ciphertext, true, new CryptoPP::PK_DecryptorFilter(rng, d, new CryptoPP::StringSink(decrypted)));

	return decrypted;
}




// Generates AES symmetric key into 16-byte uint8_t array.
// AES encryption uses a secret key of  a variable length 128-bit.
void genAesKey(uint8_t key[AES_KEYSIZE])
{
	memset(key, 0x00, AES_KEYSIZE);

	for (size_t i = 0; i < AES_KEYSIZE; i += 4)
		_rdrand32_step(reinterpret_cast<unsigned int*>(&key[i]));
}

// Encrypt plain text string using AES key
std::string encryptAesString(uint8_t key[AES_KEYSIZE], std::string plaintext)
{
	std::string ciphertext;
	uint8_t iv[AES_KEYSIZE];
	memset(iv, 0x00, AES_KEYSIZE);
	CryptoPP::AES::Encryption aesEncryption(key, AES_KEYSIZE);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
	stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
	stfEncryptor.MessageEnd();
	return ciphertext;
}

// Decrypt ciphertext string using AES key
std::string decryptAesString(uint8_t key[AES_KEYSIZE], std::string ciphertext)
{
	std::string decryptedtext;
	uint8_t iv[AES_KEYSIZE];
	memset(iv, 0x00, AES_KEYSIZE);
	CryptoPP::AES::Decryption aesDecryption(key, AES_KEYSIZE);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
	stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
	stfDecryptor.MessageEnd();

	return decryptedtext;
}


// Encrypt file using AES key
std::string encryptAesFile(uint8_t key[AES_KEYSIZE], std::string filename_in) {
	std::string ciphertext;
	uint8_t iv[AES_KEYSIZE];
	memset(iv, 0x00, AES_KEYSIZE);
	CryptoPP::AES::Encryption aesEncryption(key, AES_KEYSIZE);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

	std::string filename_out = ".encrypted" + filename_in + genRandomString();
	std::ifstream in(filename_in, std::ios::binary);
	std::ofstream out(filename_out, std::ios::binary);

	CryptoPP::FileSource encryptFile(in, true, 
		new CryptoPP::StreamTransformationFilter(cbcEncryption, new CryptoPP::FileSink(out)));

	in.close();
	out.close();
	return filename_out;
}

// Decrypt file using AES key
std::string decryptAesFile(uint8_t key[AES_KEYSIZE], std::string filename_in) {
	std::string ciphertext;
	uint8_t iv[AES_KEYSIZE];
	memset(iv, 0x00, AES_KEYSIZE);
	CryptoPP::AES::Decryption aesDecryption(key, AES_KEYSIZE);
	CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

	std::string filename_out = "%TMP%\\" + genRandomString();
	std::ifstream in(filename_in, std::ios::binary);
	std::ofstream out(filename_out, std::ios::binary);

	CryptoPP::FileSource decryptFile(in, true, new CryptoPP::StreamTransformationFilter(cbcDecryption, new CryptoPP::FileSink(out)));

	in.close();
	out.close();
	return filename_out;
}