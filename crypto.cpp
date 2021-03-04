#include <rsa.h>
#include <osrng.h>
#include <base64.h>
#include <files.h>

#include <string>
#include <fstream>

#include "crypto.h"

// Generate key pair (private and public), saves the private key to disk in base64 format and returns the public key
void GenRsaKeyPair(uint8_t public_key[])
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

	static const size_t KEYSIZE = 160;
	CryptoPP::ArraySink as(public_key, KEYSIZE);
	pubKey.Save(as);
}