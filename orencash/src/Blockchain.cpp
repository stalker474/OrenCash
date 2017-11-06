#include "Blockchain.h"

using namespace orencash;

Blockchain::Blockchain()
{
	Db = std::make_unique<Database>("bc.db");
	Db->Connect();
	if (Db->GetBlocks().size() == 0)
	{
		//generate genesis block if blockchain empty
		Db->AddBlock(Block::GetGenesis());
	}
}


Blockchain::~Blockchain()
{
}

void Blockchain::Begin(BlockchainListenerInterface* Owner)
{
	if (CurrentLocker == Owner)
		assert(false);
	MyLock.lock();
	ThreadSafeAccess = true;
	CurrentLocker = Owner;
}

void Blockchain::End()
{
	ThreadSafeAccess = false;
	CurrentLocker = nullptr;
	MyLock.unlock();
}

void Blockchain::ReplaceChain(const BlockList& Blocks)
{
	//TODO : do not fetch all blocks just for counting!
	BlockPtrList currentBlocks = GetAllBlocks();
	if (Blocks.size() <= currentBlocks.size())
		throw BlockChainException("Blockchain is shorter than the current blockchain");

	Checkchain(Blocks);

	//Get the diverging blocks
	auto first = Blocks.begin() + currentBlocks.size();
	auto last = Blocks.end();
	BlockList newBlocks(first,last);
	for (const auto& block : newBlocks)
		AddBlockImpl(block);

	for (auto& list : Listeners)
		if(list != CurrentLocker)
		list->OnBlockchainReplaced();
	//merging done
}

void Blockchain::Checkchain(const BlockList& Blocks)
{
	if(Blocks.size())
	{
		std::shared_ptr<Block> genesis = Db->GetBlockByIndex(0);
		//check genesis
		if(Blocks.at(0).Serialize() != genesis->Serialize())
			throw BlockChainException("Genesis blocks arent the same");
		//validate chain
		for(size_t i = 1; i < Blocks.size(); ++i)
		{
			try
			{
				ValidateBlock(Blocks[i], Blocks[i - 1]);
			}
			catch(std::exception e)
			{
				std::stringstream str;
				str << "invalid block sequence, block error :";
				str << e.what();
				throw BlockChainException(str.str().c_str());
			}
		}
	}
}

void Blockchain::ValidateBlock(const Block & BlockToValidate, const Block& PreviousBlock)
{
	std::string hash = BlockToValidate.To_SHA256();

	if (PreviousBlock.GetIndex() + 1 != BlockToValidate.GetIndex())
		throw BlockChainException("Invalid previous block index");
	if (PreviousBlock.GetHash() != BlockToValidate.GetPreviousHash())
		throw BlockChainException("Invalid previous block hash");
	if (BlockToValidate.GetHash() != hash)
		throw BlockChainException("Invalid block hash");
	if (BlockToValidate.GetDifficulty() >= GetDifficulty(BlockToValidate.GetIndex()))
		throw BlockChainException("Invalid proof of work difficulty");

	for(const Transaction& transact : BlockToValidate.GetTransactions())
		ValidateTransaction(transact);

	uint64_t inputSum = 0;
	uint64_t outputSum = 0;

	for(const Transaction& trans : BlockToValidate.GetTransactions())
	{
		for(const TransactionInput& input : trans.GetData().Inputs)
			inputSum += input.Amount;
		for (const TransactionOutput& output : trans.GetData().Outputs)
			outputSum += output.Amount;
	}

	if (inputSum + MINING_REWARD > outputSum)
		throw BlockChainException("Invalid block balance");

	size_t feeTransactionsCount = 0;
	size_t rewardTransactionsCount = 0;

	for (const Transaction& trans : BlockToValidate.GetTransactions())
	{
		if (trans.GetType() == TransactionType::FEE)
			feeTransactionsCount++;
		else if (trans.GetType() == TransactionType::REWARD)
			rewardTransactionsCount++;
	}

	if (feeTransactionsCount > 1)
		throw BlockChainException("Invalid fee transaction count");

	if (rewardTransactionsCount != 1)
		throw BlockChainException("Invalid reward transaction count");
}

void Blockchain::ValidateTransaction(const Transaction & TransactionToValidate)
{
	TransactionToValidate.Validate();
	BlockPtrList& blocks = GetAllBlocks();
	//verify if transaction isnt already present in the blockchain
	for(const auto& block : blocks)
	{
		for(const Transaction& trans : block->GetTransactions())
		{
			if (trans.GetID() == TransactionToValidate.GetID())
				throw BlockChainException("Transaction already in the blockchain");
		}
	}
	//verify if all input transactions are unspent in the blockchain
	for (const auto& block : blocks)
	{
		for (const Transaction& trans : block->GetTransactions())
		{
			for(const TransactionInput& in : trans.GetData().Inputs)
			{
				for (const TransactionInput& input : TransactionToValidate.GetData().Inputs)
					if (in.Address == input.Address && (in.Index == input.Index) && (in.TransactionID == input.TransactionID))
						throw BlockChainException("Not all inputs are unspent for transaction");
			}
		}
	}
}

void Blockchain::AddBlock(const Block & NewBlock)
{
	AddBlockImpl(NewBlock);

	for(auto& list : Listeners)
		if (list != CurrentLocker)
		list->OnBlockAdded(GetLastBlock());
}

void Blockchain::AddBlockImpl(const Block & NewBlock)
{
	auto block = Db->GetLastBlock();
	Block lastBlock = *block; //TODO : try to avoid copy

	ValidateBlock(NewBlock, lastBlock);
	Db->AddBlock(NewBlock);

	for (const Transaction& trans : NewBlock.GetTransactions())
		Db->RemoveTransactionByID(trans.GetID());
}

void Blockchain::AddTransaction(const Transaction & NewTransaction)
{
	uint64_t transID = NewTransaction.GetID();
	Db->AddTransaction(NewTransaction);
	auto trans = GetTransactionById(transID);
	for (auto& list : Listeners)
		if (list != CurrentLocker)
		list->OnTransactionAdded(trans);
}

BlockPtrList Blockchain::GetAllBlocks() const
{
	return Db->GetBlocks();
}

const std::shared_ptr<Block> Blockchain::GetBlockByIndex(const uint64_t& Index) const
{
	return Db->GetBlockByIndex(Index);
}

const std::shared_ptr<Block> Blockchain::GetBlockByHash(const std::string& Hash) const
{
	return Db->GetBlockByHash(Hash);
}

const std::shared_ptr<Block> Blockchain::GetLastBlock() const
{
	return Db->GetLastBlock();
}

uint64_t Blockchain::GetDifficulty(uint64_t Index) const
{
	//TODO : understand the original foruma :
	//Math.floor(
	//BASE_DIFFICULTY / Math.pow(
	//	Math.floor(((index || this.blocks.length) + 1) / EVERY_X_BLOCKS) + 1
	//	, POW_CURVE)
	//	)
	//	, 0);

	uint64_t TMax = -1;//TMax
	auto increment = Index / EVERY_X_BLOCKS;
	auto factor = std::pow(BASE_DIFFICULTY + increment, POW_CURVE);
	return static_cast<uint64_t>(TMax / factor);
}

const TransactionPtrList Blockchain::GetAllTransactions() const
{
	return Db->GetTransactions();
}

const std::vector<TransactionOutput> Blockchain::GetUnspentTransactionsForAddress(const std::string& Address)
{
	std::vector<TransactionOutput> TXO;
	std::vector<TransactionInput> TXI;
	std::vector<TransactionOutput> UTXO;
	auto blocks = Db->GetBlocks();

	//find all outputs and inputs referencing the address
	for (const auto& block : blocks)
	{
		auto transactions = block->GetTransactions();
		for (const auto& transaction : transactions)
		{
			for (const auto& output : transaction.GetData().Outputs)
			{
				if (output.Address == Address)
					TXO.push_back(output);
			}

			for (const auto& input : transaction.GetData().Inputs)
			{
				if (input.Address == Address)
					TXI.push_back(input);
			}
		}
	}

	//cross lists to find transaction outputs without a corresponding transaction input

	for (const auto& output : TXO)
	{
		bool found = false;
		for (const auto& input : TXI)
		{
			if (input.TransactionID == output.TransactionID && input.Index == output.Index)
			{
				found = true;
				break;
			}
		}
		if (!found)
			UTXO.push_back(output);
	}

	return UTXO;
}

const std::shared_ptr<Transaction> Blockchain::GetTransactionById(uint64_t ID) const
{
	return Db->GetTransactionById(ID);
}

void Blockchain::Subscribe(BlockchainListenerInterface * NewListener)
{
	Listeners.insert(Listeners.end(),NewListener);
}

void Blockchain::Unsubscribe(BlockchainListenerInterface * NewListener)
{
	Listeners.remove(NewListener);
}
