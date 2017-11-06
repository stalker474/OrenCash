#include "Miner.h"

#include <random>
#include <future>
#include "CryptoHelpers.h"
#include <iostream>

using namespace orencash;

Miner::Miner(std::shared_ptr<Blockchain> MyBlockchain) : MyBlockchain(MyBlockchain)
{
	
}

void Miner::Mine(const std::string& Address)
{
	if (Address.size() == 0)
		throw MinerException("Miner has no address to get rewarded to");
	MyBlockchain->Subscribe(this);
	MyBlockchain->Begin(this);
	Block baseBlock = GenerateNextBlock(Address, MyBlockchain->GetLastBlock(), MyBlockchain->GetAllTransactions());
	uint64_t diff = MyBlockchain->GetDifficulty(baseBlock.GetIndex());
	MyBlockchain->End();
	MyBlockchain->Unsubscribe(this);
	NewBlockReady(ProveWorkFor(baseBlock, diff), Address);
}

void Miner::Start(const std::string& Address)
{
	
	if(!IsRunning)
	{
		IsRunning = true;
		if (MinerThread != nullptr)
			MinerThread->join();
		MinerThread = nullptr;
		MinerThread = std::make_unique<std::thread>([&]()
		{
			std::string addr = Address; //copy before it disappears after this function exits
			while (IsRunning)
			{
				try
				{
					Mine(addr);
				}
				catch (std::exception& e)
				{
					std::cout << e.what();
				}
			}

			LOG("Miner done");	
		});
	}
}

void Miner::Stop()
{
	IsRunning = false;
}

void Miner::NewBlockReady(const Block& NewBlock, const std::string& Address)
{
	MyBlockchain->Begin(this);
	try
	{
		MyBlockchain->AddBlock(NewBlock);
		uint64_t Max = -1;
		LOG("New block mined! " << NewBlock.GetHash() << " " << NewBlock.GetNonce() << " diff: " << Max / NewBlock.GetDifficulty());
	}
	catch(const std::exception& e)
	{
		LOG("Mined block got rejected: " << e.what());
	}
	
	MyBlockchain->End();
}

Block Miner::GenerateNextBlock(const std::string& Address, const std::shared_ptr<Block> PreviousBlock, const TransactionPtrList& Transactions)
{
	uint64_t nextIndex = PreviousBlock->GetIndex() + 1;
	std::string previousHash = PreviousBlock->GetHash();
	time_t timestamp = time(0);

	TransactionList transactionsToWorkOn;

	uint64_t totalFee = 0;

	size_t i = 0;
	//only work if enough transactions to generate block
	for (const auto& transaction : Transactions)
	{
		if (i >= MIN_TRANSACTION_COUNT_PER_BLOCK)
			break; //max reached
		transactionsToWorkOn.emplace_back(*transaction);
		totalFee += transaction->ComputeInputAmount() - transaction->ComputeOutputAmount();
		i++;
	}
		
	if(Address != "")
	{
		//create fee transaction
		//generate fee output
		//fee should be dependent of the transaction size instead of amount
		//do not fee if no transactions
		if(totalFee > 0)
		{
			uint64_t id = CryptoHelp::GenerateRandomID();
			transactionsToWorkOn.emplace_back(id, "", TransactionType::FEE);
			transactionsToWorkOn.back().AddOutputs({ { id, 0 , totalFee, Address } });
			transactionsToWorkOn.back().ComputeHash();
		}
		
		//create reward transaction
		uint64_t id = CryptoHelp::GenerateRandomID();
		transactionsToWorkOn.emplace_back(id, "", TransactionType::REWARD);
		transactionsToWorkOn.back().AddOutputs({ { id, 1, MINING_REWARD, Address } });
		transactionsToWorkOn.back().ComputeHash();
	}
	
	Block newBlock(nextIndex,previousHash,timestamp,0,"");
	newBlock.AddTransactions(transactionsToWorkOn);

	return newBlock;
}

Block Miner::ProveWorkFor(const Block & MyBlock, uint64_t Difficulty)
{
	Block block = MyBlock; //copy ok
	uint64_t blockDifficulty = 0;
	time_t startTime = time(0);
	do {
		block.SetTimestamp(time(0));
		block.IncrementNonce();
		block.SetHash(block.To_SHA256());
		blockDifficulty = block.GetDifficulty();
	} while (blockDifficulty >= Difficulty);
	return block;
}