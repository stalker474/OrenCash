#ifndef CRYPTOHELPERS_H
#define CRYPTOHELPERS_H

#include <sha.h>
#include <hex.h>
#include <ed25519.h>
#include <pwdbased.h>
#include <random>
#include <assert.h>

using byte = unsigned char;

const std::string SALT = "0ffaa74d206930aaece253f090c88dbe6685b9e66ec49ad988d84fd7dff230d1";

namespace CryptoHelp
{
	static std::string GenerateSHA256(const std::string& data)
	{
		byte digest[CryptoPP::SHA256::DIGESTSIZE];

		CryptoPP::SHA256().CalculateDigest(digest, reinterpret_cast<const byte*>(data.c_str()), data.size());
		CryptoPP::HexEncoder encoder;
		std::string output;
		encoder.Attach(new CryptoPP::StringSink(output));
		encoder.Put(digest, sizeof(digest));
		return output;
	}

	static std::string GeneratePBKDF2(const std::string& password)
	{
		CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512> pbkdf2;

		byte derived[32];
		CryptoPP::HexDecoder decoder;
		std::string output;
		decoder.Attach(new CryptoPP::StringSink(output));
		decoder.Put(reinterpret_cast<const byte*>(password.c_str()), password.size());

		pbkdf2.DeriveKey(derived, 32, 0, reinterpret_cast<const byte*>(output.c_str()), output.size(), reinterpret_cast<const byte*>(SALT.c_str()), SALT.size(),1);
		
		output = "";
		CryptoPP::HexEncoder encoder;
		encoder.Attach(new CryptoPP::StringSink(output));
		encoder.Put(derived, 32);
		return output;
	}

	static std::pair<std::string,std::string> GenerateKeypairFromSecret(const std::string& secret)
	{
		byte pubKey[32];
		byte privKey[64];
		assert(secret.size() == 32);
		byte seed[32];
		memcpy(seed, secret.c_str(), 32);
		ed25519_create_keypair(pubKey, privKey, seed);

		std::string publicHex, privateHex;

		CryptoPP::HexEncoder encoder;
		encoder.Attach(new CryptoPP::StringSink(publicHex));
		encoder.Put(pubKey, sizeof(pubKey));

		encoder.Attach(new CryptoPP::StringSink(privateHex));
		encoder.Put(privKey, sizeof(privKey));

		return {publicHex,privateHex};
	}

	static std::string SignHash(const std::pair<std::string, std::string>& Keypair, const std::string& Message)
	{
		byte signature[64];

		std::string pubKey;
		std::string privKey;
		std::string message;

		CryptoPP::HexDecoder decoder;
		decoder.Attach(new CryptoPP::StringSink(pubKey));
		decoder.Put(reinterpret_cast<const byte*>(Keypair.first.c_str()), Keypair.first.size());
		decoder.Attach(new CryptoPP::StringSink(privKey));
		decoder.Put(reinterpret_cast<const byte*>(Keypair.second.c_str()), Keypair.second.size());
		decoder.Attach(new CryptoPP::StringSink(message));
		decoder.Put(reinterpret_cast<const byte*>(Message.c_str()), Message.size());

		ed25519_sign(signature, 
			reinterpret_cast<const byte*>(message.c_str()), message.size(), 
			reinterpret_cast<const byte*>(pubKey.c_str()),
			reinterpret_cast<const byte*>(privKey.c_str()));

		CryptoPP::HexEncoder encoder;
		std::string output;
		encoder.Attach(new CryptoPP::StringSink(output));
		encoder.Put(signature, sizeof(signature));
		return output;
	}

	static bool VerifySignature(const std::string& Address, const std::string& Signature, const std::string& Hash)
	{
		std::string signature,hash,address;
		CryptoPP::HexDecoder decoder;
		decoder.Attach(new CryptoPP::StringSink(signature));
		decoder.Put(reinterpret_cast<const byte*>(Signature.c_str()), Signature.size());
		decoder.Attach(new CryptoPP::StringSink(hash));
		decoder.Put(reinterpret_cast<const byte*>(Hash.c_str()), Hash.size());
		decoder.Attach(new CryptoPP::StringSink(address));
		decoder.Put(reinterpret_cast<const byte*>(Address.c_str()), Address.size());
		int res = ed25519_verify(reinterpret_cast<const byte*>(signature.c_str()), reinterpret_cast<const byte*>(hash.c_str()),hash.size(), reinterpret_cast<const byte*>(address.c_str()));
		return res == 1;
	}

	static uint64_t GenerateRandomID()
	{
		uint64_t val;
		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<unsigned long long> dis;
		val = dis(gen);
		val &= 0x1FFFFFFFFFFFFFFF; //so far for db storage we dont want values > 2^63
		return val;
	}
}

#endif
