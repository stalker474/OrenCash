#include "Transaction.h"

#include <iostream>

using namespace orencash;

Transaction::Transaction(const uint64_t& ID, const std::string& Hash, const TransactionType& Type):
	ID(ID),
	Hash(Hash),
	Type(Type)
{
}

Transaction::Transaction(const Transaction & Copy) : Transaction(Copy.GetID(),Copy.GetHash(),Copy.GetType())
{
	//TODO : strange range crash, look for mem issues
	//Data.Inputs.insert(Data.Inputs.end(), Copy.GetData().Inputs.begin(), Copy.GetData().Inputs.end());
	//Data.Outputs.insert(Data.Outputs.end(), Copy.GetData().Outputs.begin(), Copy.GetData().Outputs.end());

	for (const auto& val : Copy.GetData().Inputs)
		Data.Inputs.push_back(val);

	for (const auto& val : Copy.GetData().Outputs)
		Data.Outputs.push_back(val);
}

uint64_t Transaction::ComputeOutputAmount() const
{
	uint64_t amount = 0;
	for (const auto& output : Data.Outputs)
		amount += output.Amount;
	return amount;
}

uint64_t Transaction::ComputeInputAmount() const
{
	uint64_t amount = 0;
	for (const auto& input : Data.Inputs)
		amount += input.Amount;
	return amount;
}

std::string Transaction::To_SHA256() const
{
	std::ostringstream oss;
	boost::archive::text_oarchive oa(oss);

	TransactionForSHA tranSHA;
	tranSHA.Data = Data;
	tranSHA.ID = ID;
	tranSHA.Type = Type;
	oa << tranSHA;

	auto res = CryptoHelp::GenerateSHA256(oss.str());
	return res;
}

void Transaction::ComputeHash()
{
	Hash = To_SHA256();
}

std::string Transaction::Serialize() const
{
	std::ostringstream oss;
	{
		boost::archive::text_oarchive oa(oss);
		oa << *this;
	}
	return oss.str();
}

void Transaction::Validate() const
{
	if (Hash != To_SHA256())
		throw std::exception("Invalid transaction hash");

	for(const TransactionInput& input : Data.Inputs)
	{

		bool isValidSignature = CryptoHelp::VerifySignature(input.Address, input.Signature, input.To_SHA256());
		if (!isValidSignature)
			throw std::exception("Invalid transaction input signature");
	}

	if(Type == TransactionType::REGULAR)
	{
		//check if sum of input transactions is greater than output transactions to leave some transaction fees
		uint64_t sumInputs = 0;
		uint64_t sumOutputs = 0;
		for (const TransactionInput& input : Data.Inputs)
			sumInputs += input.Amount;
		for (const TransactionOutput& output : Data.Outputs)
			sumOutputs += output.Amount;

		if (!(sumInputs >= sumOutputs))
			throw std::exception("Invalid transaction balance");

		if (sumInputs - sumOutputs < MINIMUM_TRANSACTION_FEES)
			throw std::exception("Not enough fee");
	}
}

void Transaction::AddOutputs(const std::vector<TransactionOutput>& Outputs)
{
	Data.Outputs.insert(Data.Outputs.end(), Outputs.begin(), Outputs.end());
}

void Transaction::AddInputs(const std::vector<TransactionInput>& Inputs)
{
	Data.Inputs.insert(Data.Inputs.end(), Inputs.begin(), Inputs.end());
}
