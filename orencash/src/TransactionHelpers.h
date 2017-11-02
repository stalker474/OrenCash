#ifndef TRANSACTIONHELPERS_H
#define TRANSACTIONHELPERS_H

#include <stdint.h>
#include <string>
#include "Transaction.h"
#include "CryptoHelpers.h"

using namespace orencash;

namespace TransactionHelp
{
	class TransactionBuilder
	{
	public:

		TransactionBuilder() 
		{
			WalletID = 0;
			FromAddress = "";
			ToAddress = "";
			TotalAmount = 0;
			FeeAmount = 0;
			ChangeAddress = "";
			SecretKey = "";
		};

		void From(const std::vector<TransactionOutput>& UTXO)
		{
			UTXOs = UTXO;
		}

		void To(const std::string& Address, const uint64_t Amount)
		{
			ToAddress = Address;
			TotalAmount = Amount;
		}

		void Change(const std::string& Address)
		{
			ChangeAddress = Address;
		}

		void Fee(const uint64_t& Amount)
		{
			FeeAmount = Amount;
		}

		void Sign(const std::string& Secret)
		{
			SecretKey = Secret;
		}

		void Sign2(const std::pair<std::string,std::string>& Keypair)
		{
			Key = Keypair;
		}

		void Type(TransactionType Type)
		{
			MyType = Type;
		}

		Transaction Build()
		{
			if (UTXOs.size() == 0)
				throw std::exception("Must have UTXO list");
			if(ToAddress.size() == 0)
				throw std::exception("Must have a destination address");
			if (TotalAmount == 0)
				throw std::exception("Cannot be a 0 amount transaction");

			Transaction transaction(CryptoHelp::GenerateRandomID(), "", MyType);

			//Calculate change amount
			uint64_t Amount = 0;
			for (const auto& output : UTXOs)
				Amount += output.Amount;

			if (TotalAmount > Amount - FeeAmount)
				throw std::exception("Transaction is spending more than it has");

			uint64_t ChangeAmount = Amount - TotalAmount - FeeAmount;

			//generate inputs from outputs
			for (const auto& utxo : UTXOs)
			{
				TransactionInput input;
				input.TransactionID = utxo.TransactionID;
				input.Index = utxo.Index;
				input.Address = utxo.Address;
				input.Amount = utxo.Amount;

				std::string hash = input.To_SHA256();
				input.Signature = CryptoHelp::SignHash(Key,hash);

				transaction.AddInputs({ input });
			}

			//add target receiver
			TransactionOutput receiverOutput;
			receiverOutput.Amount = TotalAmount;
			receiverOutput.Address = ToAddress;
			receiverOutput.Index = 0;
			receiverOutput.TransactionID = transaction.GetID();
			transaction.AddOutputs({ receiverOutput });

			if(ChangeAmount > 0)
			{
				//add change amount
				TransactionOutput changeOutput;
				changeOutput.Amount = ChangeAmount;
				changeOutput.Address = ChangeAddress;
				changeOutput.Index = 1;
				changeOutput.TransactionID = transaction.GetID();
				transaction.AddOutputs({ changeOutput });
			}
			
			transaction.ComputeHash();

			return transaction;
		}

	private:
		std::vector<TransactionOutput>		UTXOs;
		uint64_t							WalletID;
		std::string							FromAddress;
		std::string							ToAddress;
		std::string							ChangeAddress;
		uint64_t							TotalAmount;
		uint64_t							FeeAmount;
		std::string							SecretKey;
		std::pair<std::string, std::string> Key;
		TransactionType						MyType;
	};
}

#endif
