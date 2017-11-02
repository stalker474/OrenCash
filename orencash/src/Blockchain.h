#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "Database.h"
#include "Config.h"
#include <memory>

namespace orencash
{
	class BlockchainListenerInterface abstract
	{
	public:
		virtual void OnBlockAdded(std::shared_ptr<Block> NewBlock) = 0;
		virtual void OnTransactionAdded(std::shared_ptr<Transaction> NewTransaction) = 0;
		virtual void OnBlockchainReplaced() = 0;
	};

	class Blockchain
	{
	public:
		Blockchain();
		~Blockchain();

		void								Begin(BlockchainListenerInterface* Owner);
		void								End();
		void								ReplaceChain(const BlockList& Blocks);
		void								ValidateBlock(const Block& BlockToValidate, const Block& PreviousBlock);
		void								ValidateTransaction(const Transaction& TransactionToValidate);
		void								AddBlock(const Block& NewBlock);
		void								AddTransaction(const Transaction& NewTransaction);

		BlockPtrList						GetAllBlocks() const;
		const std::shared_ptr<Block>		GetBlockByIndex(const uint64_t& Index) const;
		const std::shared_ptr<Block>		GetBlockByHash(const std::string& Hash) const;
		const std::shared_ptr<Block>		GetLastBlock() const;

		uint64_t							GetDifficulty(uint64_t Index) const;

		const TransactionPtrList			GetAllTransactions() const;
		const std::vector<TransactionOutput>GetUnspentTransactionsForAddress(const std::string& Address);
		const std::shared_ptr<Transaction>	GetTransactionById(uint64_t ID) const;

		void								Subscribe(BlockchainListenerInterface* NewListener);
		void								Unsubscribe(BlockchainListenerInterface* NewListener);

	private:
		void								Checkchain(const BlockList& Blocks);
		void								AddBlockImpl(const Block& NewBlock);

		std::list<BlockchainListenerInterface*>		Listeners;
		BlockchainListenerInterface*				CurrentLocker;
		std::unique_ptr<Database>					Db;
		std::mutex									MyLock;
		bool										ThreadSafeAccess;
	};
}

#endif

