#ifndef MINER_H
#define MINER_H

#include "Blockchain.h"
#include <memory>

namespace orencash
{
	class Miner : public BlockchainListenerInterface
	{
	public:
		Miner(std::shared_ptr<Blockchain> MyBlockchain);

		void		Mine(const std::string& Address);
		void		Start(const std::string& Address);
		void		Stop();

		void		NewBlockReady(const Block& NewBlock, const std::string& Address);

		inline bool IsMining() const { return IsRunning; }

		// Inherited via BlockchainListenerInterface
		virtual void OnBlockAdded(std::shared_ptr<Block> NewBlock) override
		{}
		virtual void OnTransactionAdded(std::shared_ptr<Transaction> NewTransaction) override
		{}
		virtual void OnBlockchainReplaced() override
		{}
		
	private:
		Block		 GenerateNextBlock(const std::string& Address, const std::shared_ptr<Block> PreviousBlock, const TransactionPtrList& Transactions);
		static Block ProveWorkFor(const Block& MyBlock, uint64_t Difficulty);

		std::shared_ptr<Blockchain>		MyBlockchain;
		std::mutex						BCAccessMutex;
		std::shared_ptr<std::thread>	MinerThread;
		std::atomic<bool>				IsRunning;
	};
}

#endif

