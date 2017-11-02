#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <stdint.h>
#include <vector>
#include <sstream>
#include "CryptoHelpers.h"
#include "Config.h"

#include <boost\archive\text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace orencash
{

	enum class TransactionType : short
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

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & TransactionID;
			archive & Index;
			archive & Address;
			archive & Amount;
		}
	};

	struct TransactionInput
	{
		uint64_t	TransactionID;
		uint64_t	Index;
		uint64_t	Amount;
		std::string Address;
		std::string Signature;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & TransactionID;
			archive & Index;
			archive & Address;
			archive & Amount;
			archive & Signature;
		}

		std::string To_SHA256() const
		{
			TransactionInputForSHA256 data =
			{
				TransactionID,
				Index,
				Amount,
				Address
			};
			std::ostringstream oss;
			{
				boost::archive::text_oarchive oa(oss);
				oa << data;
			}
			return CryptoHelp::GenerateSHA256(oss.str());
		}
	};

	struct TransactionOutput
	{
		uint64_t	TransactionID;
		uint64_t	Index;
		uint64_t	Amount;
		std::string Address;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & TransactionID;
			archive & Index;
			archive & Amount;
			archive & Address;
		}

		std::string To_SHA256() const
		{
			std::ostringstream oss;
			{
				boost::archive::text_oarchive oa(oss);
				oa << *this;
			}
			return CryptoHelp::GenerateSHA256(oss.str());
		}
	};

	struct DataContent
	{
		std::vector<TransactionInput>	Inputs;
		std::vector<TransactionOutput>	Outputs;

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & Inputs;
			archive & Outputs;
		}
	};

	class Transaction
	{
	public:
		Transaction() 
		{}
		Transaction(const uint64_t& ID, const std::string& Hash, const TransactionType& Type);
		Transaction(const Transaction& Copy);

		inline uint64_t				GetID() const noexcept { return ID; }
		inline std::string			GetHash() const noexcept { return Hash; }
		inline TransactionType		GetType() const noexcept { return Type; }
		inline const DataContent	GetData() const noexcept { return Data; }

		uint64_t					ComputeOutputAmount() const;
		uint64_t					ComputeInputAmount() const;

		std::string					To_SHA256() const;
		std::string					Serialize() const;

		void						ComputeHash();
		void						Validate() const;

		void						AddOutputs(const std::vector<TransactionOutput>& Outputs);
		void						AddInputs(const std::vector<TransactionInput>& Inputs);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & ID;
			archive & Type;
			archive & Hash;
			archive & Data;
		}

		uint64_t					ID;
		std::string					Hash;
		TransactionType				Type;
		DataContent					Data;
	};

	using TransactionList = std::vector<Transaction>;
	using TransactionPtrList = std::list<std::shared_ptr<Transaction>>;

	struct TransactionForSHA
	{
	public:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & ID;
			archive & Type;
			archive & Data;
		}
		uint64_t					ID;
		TransactionType				Type;
		DataContent					Data;
	};
}

#endif