#include "Wallet.h"

#include "CryptoHelpers.h"

#include <algorithm>
#include <random>

using namespace orencash;

Wallet::Wallet()
{
	ID = 0;
	PasswordHash = "";
	Secret = "";
}

orencash::Wallet::Wallet(const Wallet & Copy)
{
	ID = Copy.GetID();
	PasswordHash = Copy.GetPasswordHash();
	Secret = Copy.GetSecret();
	Keypairs.insert(Keypairs.end(), Copy.GetKeypairs().begin(), Copy.GetKeypairs().end());
}

orencash::Wallet::Wallet(const std::string& Password) : Wallet()
{
	ID = CryptoHelp::GenerateRandomID();
	PasswordHash = CryptoHelp::GenerateSHA256(Password);
}

orencash::Wallet::Wallet(const std::string& PasswordHash, bool dummy) : Wallet()
{
	ID = CryptoHelp::GenerateRandomID();
	this->PasswordHash = PasswordHash;
}

orencash::Wallet::Wallet(const uint64_t & ID, const std::string & PasswordHash, const std::string & Secret) : 
	ID(ID),
	PasswordHash(PasswordHash),
	Secret(Secret)
{

}

std::string orencash::Wallet::GenerateAddress()
{
	if (Secret.size() == 0)
		GenerateSecret();

	auto last_keyPair = Keypairs.size()>0?Keypairs.end()-1:Keypairs.end();

	//generate next seed based on the first secret or a new secret from the last key pair
	std::string seed = (last_keyPair == Keypairs.end()) ? Secret : CryptoHelp::GeneratePBKDF2(last_keyPair->first);
	//generate keypair from seed
	Keypairs.push_back(CryptoHelp::GenerateKeypairFromSecret(seed));
	return (Keypairs.end() - 1)->first;
}

std::string orencash::Wallet::GetAddressByIndex(const uint64_t & Index) const
{
	return Keypairs[Index].first;
}

std::string orencash::Wallet::GetAddressByPublicKey(const std::string & publicKey) const
{
	auto res = std::find_if(Keypairs.begin(), Keypairs.end(), 
		[&](auto& pair) 
	{
		return pair.first == publicKey;
	});
	if (res == Keypairs.end())
		throw std::exception("Public key not found");
	return res->first;
}

std::string orencash::Wallet::GetSecretKeyByAddress(const std::string & Address) const
{
	auto res = std::find_if(Keypairs.begin(), Keypairs.end(), 
		[&](auto& pair) 
	{
		return pair.first == Address;
	});
	if (res == Keypairs.end())
		throw std::exception("Public key not found");
	return res->second;
}

std::vector<std::string> orencash::Wallet::GetAddresses() const
{
	std::vector<std::string> addresses;
	for (const auto& keypair : Keypairs)
		addresses.push_back(keypair.second);
	return addresses;
}

void Wallet::AddKeypair(const KeyPair & Pair)
{
	Keypairs.push_back(Pair);
}

std::string orencash::Wallet::GenerateSecret()
{
	Secret = CryptoHelp::GeneratePBKDF2(PasswordHash);
	return Secret;
}
