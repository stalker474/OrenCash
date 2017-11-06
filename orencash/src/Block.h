#ifndef BLOCK_H
#define BLOCK_H

#include "Transaction.h"

#include <stdint.h>
#include <time.h>
#include <boost/archive/text_oarchive.hpp>

namespace orencash
{
	class Block
	{
	public:
		Block();
		Block(const uint64_t&		Index, 
			const std::string&		PreviousHash, 
			const time_t&			TimeStamp, 
			const uint64_t&			Nonce, 
			const std::string&		Hash);
		Block(const Block&			Copy);

        inline static Block				GetGenesis() noexcept { return Block(); }

        inline const uint64_t			GetIndex() const noexcept { return Index; }
        inline const std::string		GetPreviousHash() const noexcept { return PreviousHash; }
        inline const time_t				GetTimeStamp() const noexcept { return TimeStamp; }
        inline const uint64_t			GetNonce() const noexcept { return Nonce; }
        inline const std::string		GetHash() const noexcept { return Hash; }
		inline const TransactionList&	GetTransactions() const noexcept { return Transactions; }
		const Transaction&				GetTransactionById(const uint64_t& Id) const;
		uint64_t						GetDifficulty() const noexcept;

		const std::string				To_SHA256() const;
		const std::string				Serialize() const;

		void							AddTransactions(const TransactionList& NewTransactions);
		void							SetTimestamp(time_t Timestamp);
		void							SetHash(std::string Hash);
		void							IncrementNonce();

	private:
		uint64_t			Index;
		std::string			PreviousHash;
		time_t				TimeStamp;
		uint64_t			Nonce;
		std::string			Hash;
		TransactionList		Transactions;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & Index;
			archive & Hash;
			archive & PreviousHash;
			archive & TimeStamp;
			archive & Transactions;
			archive & Nonce;
		}
	};

	using BlockList = std::vector<Block>;
	using BlockPtrList = std::vector<std::shared_ptr<Block>>;

	class BlockException : public std::exception
	{
	public:
		BlockException(const std::string& msg)
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
