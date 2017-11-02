#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <exception>

#include "Block.h"
#include "Transaction.h"
#include "Wallet.h"

#include <set>

#ifdef WITH_ODB
#include <odb/database.hxx>
#endif

namespace orencash
{

	using AddressPair = std::pair<std::string, std::string>;
	using WalletPtrList = std::vector<std::shared_ptr<Wallet>>;

	class Database
	{
	public:
		explicit Database(std::string Name)
			: Name(Name)
			,ConnectionObject(nullptr) 
		{};

		bool								Connect(const std::string& User="", const std::string& Password="") noexcept;
		bool								Disconnect();
		void								AddBlock(const Block& NewBlock);
		void								AddTransaction(const Transaction& NewTransaction);
		void								AddWallet(const Wallet& NewWallet);
		void								UpdateWallet(const Wallet& TheWallet);
		void								RemoveTransactionByID(uint64_t ID);
		void								ValidateConnection();


		const std::vector<AddressPair>		GetAddressesForWallet(const uint64_t& ID);
		const std::shared_ptr<Block>		GetBlockByIndex(uint64_t Index) const;
		const std::shared_ptr<Block>		GetBlockByHash(std::string Hash) const;
		const std::shared_ptr<Block>		GetLastBlock() const;
		const std::shared_ptr<Transaction>	GetTransactionById(const uint64_t& ID) const;
		const std::shared_ptr<Wallet>		GetWalletById(const uint64_t& ID);

		inline const BlockPtrList			GetBlocks() const { return Blocks; }
		inline const WalletPtrList			GetWallets() const { return Wallets; }
		inline const TransactionPtrList		GetTransactions() const { return Transactions; }


	private:
		void								Persist(const Block& NewBlock);
		void								Persist(const Wallet& NewWallet);

		std::string								Name;
#ifdef WITH_ODB
		std::unique_ptr<odb::core::database>	ConnectionObject;	
#endif
		BlockPtrList							Blocks;
		TransactionPtrList						Transactions;
		WalletPtrList							Wallets;
	};
}

#endif