#include "Block.h"

#include <sha.h>
#include <iostream>
#include <sstream>

#include "CryptoHelpers.h"

using namespace orencash;

struct BlockForSHA
{
public:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & archive, const unsigned int file_version)
	{
		archive & Index;
		archive & PreviousHash;
		archive & TimeStamp;
		archive & Transactions;
		archive & Nonce;
	}

	uint64_t					Index;
	std::string					PreviousHash;
	time_t						TimeStamp;
	uint64_t					Nonce;

	std::vector<Transaction>	Transactions;
};

Block::Block(const uint64_t & Index, const std::string & PreviousHash, const time_t & TimeStamp, const uint64_t & Nonce, const std::string & Hash) :
	Index(Index),
	PreviousHash(PreviousHash),
	TimeStamp(TimeStamp),
	Nonce(Nonce),
	Hash(Hash)
{
}

Block::Block()
{
	Index = 0;
	PreviousHash = "";
	TimeStamp = 1465154705;
	Nonce = 0;
	Hash = To_SHA256();
}

Block::Block(const Block & Copy) : Block(Copy.GetIndex(),Copy.GetPreviousHash(),Copy.GetTimeStamp(),Copy.GetNonce(),Copy.GetHash())
{
	for (const auto& val : Copy.GetTransactions())
		Transactions.push_back(val);
	// TODO : weird range crash
	//Transactions.insert(Transactions.begin(), Copy.GetTransactions().begin(), Copy.GetTransactions().end());
}

const Transaction & Block::GetTransactionById(const uint64_t & Id) const
{
    const auto& res = std::find_if(Transactions.begin(),Transactions.end(),
		[Id](const Transaction& transaction)
	{
		return transaction.GetID() == Id;
	});

	if (res != Transactions.end())
		return *res;
	else
		throw BlockException("Transaction id not found");
}

const std::string Block::To_SHA256() const
{
	std::ostringstream oss;
	{
		boost::archive::text_oarchive oa(oss);
		BlockForSHA block = 
		{
			Index,
			PreviousHash,
			TimeStamp,
			Nonce,
			Transactions
		};
		oa << block;
	}
	return CryptoHelp::GenerateSHA256(oss.str());
}

uint64_t Block::GetDifficulty() const noexcept
{
	//TODO : send something along parseInt(this.hash.substring(0, 14), 16)
	uint64_t diff;
	std::stringstream ss;
	std::string Hash16bytes;
	Hash16bytes.insert(Hash16bytes.end(),Hash.begin(),Hash.begin()+16);
	ss << std::hex << Hash16bytes;
	ss >> diff;

	return diff;
}

const std::string Block::Serialize() const
{
	std::ostringstream oss;
	{
		boost::archive::text_oarchive oa(oss);
		oa << *this;
	}
	return oss.str();
}

void Block::AddTransactions(const TransactionList& NewTransactions)
{
	Transactions.insert(Transactions.end(), NewTransactions.begin(), NewTransactions.end());
}

void Block::SetTimestamp(time_t NewTimestamp)
{
	TimeStamp = NewTimestamp;
}

void Block::SetHash(std::string NewHash)
{
	Hash = NewHash;
}

void Block::IncrementNonce()
{
	Nonce++;
}
