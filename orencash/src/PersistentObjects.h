#ifndef PERSIST_H
#define PERSIST_H

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

#include "Config.h"

#include <odb/core.hxx>

namespace orencash
{
	namespace persist
	{
		enum TransactionType
		{
			REGULAR = 0,
			FEE = 1,
			REWARD = 2
		};

		struct TransactionInputForSHA256
		{
			uint64_t	TransactionID;
			uint64_t	Index;
			uint64_t	Amount;
			std::string Address;
		};
#pragma db value
		struct TransactionInput
		{
			uint64_t	TransactionID;
			uint64_t	Index;
			uint64_t	Amount;
			std::string Address;
			std::string Signature;
		};
#pragma db value
		struct TransactionOutput
		{
			uint64_t	TransactionID;
			uint64_t	Index;
			uint64_t	Amount;
			std::string Address;
		};
#pragma db object
		class Transaction
		{
		public:
			Transaction()
			{}
#pragma db id
			uint64_t					ID;
			std::string					Hash;
			TransactionType				Type;
			std::vector<TransactionInput>	Inputs;
			std::vector<TransactionOutput>	Outputs;

		private:
			friend class odb::access;
		};
#pragma db object
		class Block
		{
		public:
			Block()
			{}
#pragma db id
			uint64_t												Index;
			std::string												PreviousHash;
			time_t													TimeStamp;
			uint64_t												Nonce;
			std::string												Hash;
			std::vector< std::shared_ptr<Transaction> >				Transactions;
		};
#pragma db value
		struct KeyPair
		{
			std::string PublicKey;
			std::string PrivateKey;
		};
#pragma db object
		class Wallet
		{
		public:
			Wallet()
			{}
#pragma db id
			uint64_t					ID;
			std::string					PasswordHash;
			std::string					Secret;
			std::vector<KeyPair>		Keypairs; //key/secretkey
		};
	}
}
#endif
