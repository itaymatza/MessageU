#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>
#include <string>
#include <fstream>
#include "crypto.h"

static const size_t KEYSIZE = 160;


// Generate key pair (private and public), saves the private key to disk in base64 format and returns the public key
void genRsaKeyPair(uint8_t public_key[])
{
	// private key generation
	CryptoPP::AutoSeededRandomPool rng;
	CryptoPP::InvertibleRSAFunction privkey;
	privkey.Initialize(rng, 1024);
	CryptoPP::Base64Encoder privkeysink(new CryptoPP::FileSink("me.info"));
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

	CryptoPP::ArraySink as(public_key, KEYSIZE);
	pubKey.Save(as);
}


// Encrypt plain text string using RSA public key
std::string encryptString(uint8_t public_key[], std::string plain_text)
{
	CryptoPP::AutoSeededRandomPool rng;

	// from uint8_t buffer to public key
	CryptoPP::ArraySource as(public_key, KEYSIZE, true);
	CryptoPP::RSA::PublicKey pubKey;
	pubKey.Load(as);

	// encrypt plain text
	std::string ciphertext;
	CryptoPP::RSAES_OAEP_SHA_Encryptor e(pubKey);
	CryptoPP::StringSource ss(plain_text, true, new CryptoPP::PK_EncryptorFilter(rng, e, new CryptoPP::StringSink(ciphertext)));

	return ciphertext;
}


// Decrypt ciphertext string using RSA private key
std::string decryptString(std::string private_key, std::string ciphertext)
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