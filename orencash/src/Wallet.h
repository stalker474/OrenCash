#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>

namespace orencash
{
	using KeyPair = std::pair<std::string, std::string>;

	class Wallet
	{
	public:
		Wallet();
		Wallet(const Wallet& Copy);
		Wallet(const std::string& Password);
		Wallet(const std::string& PasswordHash, bool dummy);
		Wallet(const uint64_t& ID, const std::string& PasswordHash, const std::string& Secret);

		std::string					GenerateAddress();

		std::string					GetAddressByIndex(const uint64_t& Index) const;
		std::string					GetAddressByPublicKey(const std::string& publicKey) const;
		std::string					GetSecretKeyByAddress(const std::string& Address) const;
		std::vector<std::string>	GetAddresses() const;

		uint64_t					GetID() const noexcept { return ID; };
		std::string					GetPasswordHash() const noexcept { return PasswordHash; };
		std::string					GetSecret() const noexcept { return Secret; };
		std::vector<KeyPair>		GetKeypairs() const noexcept { return Keypairs; };
		void						AddKeypair(const KeyPair& Pair);
	private:
		std::string					GenerateSecret();

		uint64_t					ID;
		std::string					PasswordHash;
		std::string					Secret;
		std::vector<KeyPair>		Keypairs; //key/secretkey
	};

	class WalletException : public std::exception
	{
	public:
		WalletException(const std::string& msg)
		{
			Msg = msg;
		}

        virtual char const* what() const noexcept override
		{
			return Msg.c_str();
		}

	private:
		std::string Msg;
	};
}

#endif

