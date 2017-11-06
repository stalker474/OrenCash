#ifndef OPERATOR_H
#define OPERATOR_H

#include "Database.h"
#include "Wallet.h"
#include "Blockchain.h"

namespace orencash
{
	using WalletList = std::vector <std::shared_ptr<Wallet>>;

	class Operator
	{
	public:
		Operator(std::shared_ptr<Blockchain> MyBlockchain);

		void							AddWallet(const Wallet& NewWallet);
		std::shared_ptr<Wallet>			CreateWalletFromPassword(const std::string& Password);
		std::shared_ptr<Wallet>			CreateWalletFromHash(const std::string& Hash);
		Transaction						CreateTransaction(const uint64_t& WalletID, const std::string& FromAddress, const std::string& ToAddress, const uint64_t& Amount, const uint64_t & Fee, const std::string& ChangeAddress);
		Transaction						CreateTransaction(const std::string& SecretKey, const std::string& FromAddress, const std::string& ToAddress, const uint64_t& Amount, const uint64_t & Fee, const std::string& ChangeAddress);
		bool							CheckWalletPassword(const uint64_t& WalletID, const std::string& PasswordHash);
		const std::string				GenerateAddressForWallet(const uint64_t& WalletID);

		std::shared_ptr<Wallet>			GetWalletByID(const uint64_t& WalletID);
		const WalletList				GetWallets();
		uint64_t						GetBalanceForWalletAddress(const std::string& Address);
		const std::vector<std::string>	GetAddressesForWallet(const uint64_t& WalletID);

		private:
		std::unique_ptr<Database>	Db;
		std::shared_ptr<Blockchain> MyBlockchain;
	};

	class OperatorException : public std::exception
	{
	public:
		OperatorException(const std::string& msg)
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

