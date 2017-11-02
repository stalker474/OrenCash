#include "Node.h"
#include <iostream>

using namespace orencash;

Node::Node(std::shared_ptr<Blockchain> MyBlockchain,unsigned short Port, const std::string& publicAddr) : MyBlockchain(MyBlockchain)
{
	Network = std::make_unique<P2PServer>(this,Port,publicAddr);
	Network->Subscribe(this);
}

void Node::ConnectToPeer(PeerAddress Address)
{
	Network->AddPeer(Address);
	Network->JoinKnownPeers();
}

void Node::Start()
{
	MyBlockchain->Subscribe(this);
	Network->Run();
}

const std::shared_ptr<Block> Node::GetLastBlock()
{
	std::lock_guard<std::mutex> lock(CheckBlocksMutex);
	MyBlockchain->Begin(this);
	auto block = MyBlockchain->GetLastBlock();
	MyBlockchain->End();
	return block;
}

const TransactionPtrList Node::GetTransactions()
{
	std::lock_guard<std::mutex> lock(CheckBlocksMutex);
	auto res = MyBlockchain->GetAllTransactions();
	return res;
}

const BlockPtrList Node::GetBlocks()
{
	std::lock_guard<std::mutex> lock(CheckBlocksMutex);
	MyBlockchain->Begin(this);
	auto res = MyBlockchain->GetAllBlocks();
	MyBlockchain->End();
	return res;
}

void Node::OnReceivedTransactions(const TransactionList& Transactions)
{
	MyBlockchain->Begin(this);
	
	for (auto& trans : Transactions)
	{
		try
		{
			MyBlockchain->AddTransaction(trans);
		}
		catch (std::exception& e)
		{
			LOG("Failed to accept transaction : " << e.what());
		}
	}
		
	MyBlockchain->End();
}

void Node::OnReceivedLastBlock(const Block & NewBlock)
{
	CheckReceivedBlocks({NewBlock});
}

void Node::OnReceivedBlockchain(const BlockList& Blocks)
{
	//copy the blocks here
	CheckReceivedBlocks(Blocks);
}

void Node::CheckReceivedBlocks(BlockList Blocks)
{
	LOG("Received new blockchain, analyzing");
	assert(Blocks.size());
	//order by index
	std::sort(Blocks.begin(),Blocks.end(),
		[](const Block& elem1, const Block& elem2)
	{
		return elem1.GetIndex() < elem2.GetIndex();
	});

	const Block& latestReceived = Blocks[Blocks.size() - 1];

	CheckBlocksMutex.lock();

	auto latestHeld = MyBlockchain->GetLastBlock();

	if(latestReceived.GetIndex() <= latestHeld->GetIndex())
	{
		LOG("The received block chain is not longer than my blockchain");
		CheckBlocksMutex.unlock();
		return;
	}

	LOG("Blockchain possibly behind, my last index : " << latestHeld->GetIndex() << " received got " << latestReceived.GetIndex());
	if(latestHeld->GetHash() == latestReceived.GetPreviousHash())
	{
		MyBlockchain->Begin(this);
		try
		{
			MyBlockchain->AddBlock(latestReceived);
			LOG("The received new block is appended");
		}
		catch(std::exception& e)
		{
			LOG("The block is rejected: " << e.what());
		}
		MyBlockchain->End();
	}
	else
	{
		LOG("Received blockchain is longer, replacing");
		if(Blocks.size() > 1)
		{
			MyBlockchain->Begin(this);
			try
			{
				MyBlockchain->ReplaceChain(Blocks);
			}
			catch(std::exception& e)
			{
				LOG("New chain rejected: " << e.what());
			}
			MyBlockchain->End();
			LOG("Replaced");
		}
		else
		{
			CheckBlocksMutex.unlock();
			//unlock and get the full blockchain for recheck
			Network->RequestBlockchain();
			//we are done here
			return;
		}

	}
	CheckBlocksMutex.unlock();
}

void Node::OnBlockAdded(const std::shared_ptr<Block> NewBlock)
{
	Network->SubmitBlock(NewBlock);
	LOG("SBL Broadcasted");
}

void Node::OnTransactionAdded(const std::shared_ptr<Transaction> NewTransaction)
{
	Network->SubmitTransaction(NewTransaction);
	LOG("STL Broadcasted");
}

void Node::OnBlockchainReplaced()
{
	//miner added a block
	MyBlockchain->Begin(this);
	auto block = MyBlockchain->GetLastBlock();
	MyBlockchain->End();
	if (block)
	{
		Network->SubmitBlock(block);
		LOG("SBL Broadcasted");
	}
}

void Node::OnConnected()
{
	LOG("We are online, request latest block and pending transactions");
	Network->RequestLatestBlock();
	Network->RequestTransactions();
}
