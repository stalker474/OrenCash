#ifndef NODE_H
#define NODE_H

#include "RPC.h"
#include "Blockchain.h"

namespace orencash
{
	class Node : public NetworkListenerInterface, public BlockchainListenerInterface
	{
	public:
		Node(std::shared_ptr<Blockchain> MyBlockchain,unsigned short Port,const std::string& publicAddr);
		
		void						ConnectToPeer(PeerAddress Address);
		void						Start();

		const std::shared_ptr<Block>GetLastBlock();
		const TransactionPtrList	GetTransactions();
		const BlockPtrList			GetBlocks();
	private:
		// Inherited via NetworkListenerInterface
		virtual void				OnReceivedBlockchain(const BlockList& Blocks) override;
		virtual void				OnReceivedLastBlock(const Block& Blocks) override;
		virtual void				OnReceivedTransactions(const TransactionList& Transactions) override;
		virtual void				OnConnected() override;

		// Inherited via BlockchainListenerInterface
		virtual void				OnBlockAdded(const std::shared_ptr<Block> NewBlock) override;
		virtual void				OnTransactionAdded(const std::shared_ptr<Transaction> NewTransaction) override;
		virtual void				OnBlockchainReplaced() override;

		void						CheckReceivedBlocks(BlockList Blocks);

		std::shared_ptr<Blockchain> MyBlockchain;
		std::unique_ptr<P2PServer>	Network;
		std::mutex					CheckBlocksMutex;
	};
}

#endif

