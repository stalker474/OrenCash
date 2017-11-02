#include "Operator.h"

#include "TransactionHelpers.h"

using namespace orencash;

Operator::Operator(std::shared_ptr<Blockchain> MyBlockchain) : MyBlockchain(MyBlockchain)
{
	Db = std::make_unique<Database>("wallets.db");
}

void Operator::AddWallet(const Wallet & NewWallet)
{
	Db->AddWallet(NewWallet);
}

std::shared_ptr<Wallet> Operator::CreateWalletFromPassword(const std::string& Password)
{
	Wallet wlt(Password);
	Db->AddWallet(wlt);
	return Db->GetWalletById(wlt.GetID());
}

std::shared_ptr<Wallet> Operator::CreateWalletFromHash(const std::string& Hash)
{
	Wallet wlt(Hash,true);
	Db->AddWallet(wlt);
	return Db->GetWalletById(wlt.GetID());
}

bool Operator::CheckWalletPassword(const uint64_t & WalletID, const std::string& PasswordHash)
{
	auto wlt = GetWalletByID(WalletID);
	return wlt->GetPasswordHash() == PasswordHash;
}

std::shared_ptr<Wallet> Operator::GetWalletByID(const uint64_t & WalletID)
{
	return Db->GetWalletById(WalletID);
}

const WalletList Operator::GetWallets()
{
	return Db->GetWallets();
}

const std::string Operator::GenerateAddressForWallet(const uint64_t & WalletID)
{
	auto wlt = GetWalletByID(WalletID);
	return wlt->GenerateAddress();
}

const std::vector<std::string> Operator::GetAddressesForWallet(const uint64_t & WalletID)
{
	auto wlt = GetWalletByID(WalletID);
	return wlt->GetAddresses();
}

uint64_t Operator::GetBalanceForWalletAddress(const std::string& Address)
{
	auto UTXOs = MyBlockchain->GetUnspentTransactionsForAddress(Address);

	uint64_t amount = 0;
	for (const auto& utxo : UTXOs)
		amount += utxo.Amount;

	return amount;
}

Transaction Operator::CreateTransaction(const uint64_t & WalletID, const std::string & FromAddress, const std::string & ToAddress, const uint64_t & Amount, const uint64_t & Fee, const std::string& ChangeAddress)
{
	if (Fee < MINIMUM_TRANSACTION_FEES)
		throw std::exception("Fees must be higher");
	auto UTXOs = MyBlockchain->GetUnspentTransactionsForAddress(FromAddress);

	auto wallet = GetWalletByID(WalletID);
	
	std::string secretKey = wallet->GetSecretKeyByAddress(FromAddress);
	if (secretKey.size() == 0)
		throw std::exception("Wallet secret key error");

	TransactionHelp::TransactionBuilder builder;
	builder.From(UTXOs);
	builder.To(ToAddress, Amount);
	builder.Change(ChangeAddress.size()?ChangeAddress:FromAddress);
	builder.Fee(Fee);
	//builder.Sign(secretKey);
	builder.Sign2({ FromAddress, secretKey });
	builder.Type(TransactionType::REGULAR);

	auto trans = builder.Build();
	MyBlockchain->AddTransaction(trans);
	return trans;
}

Transaction Operator::CreateTransaction(const std::string & SecretKey, const std::string & FromAddress, const std::string & ToAddress, const uint64_t & Amount, const uint64_t & Fee, const std::string & ChangeAddress)
{
	if (Fee < MINIMUM_TRANSACTION_FEES)
		throw std::exception("Fees must be higher");
	auto UTXOs = MyBlockchain->GetUnspentTransactionsForAddress(FromAddress);

	if (SecretKey.size() == 0)
		throw std::exception("Secret key error");

	TransactionHelp::TransactionBuilder builder;
	builder.From(UTXOs);
	builder.To(ToAddress, Amount);
	builder.Change(ChangeAddress.size() ? ChangeAddress : FromAddress);
	builder.Fee(Fee);
	builder.Sign2({ FromAddress, SecretKey });
	builder.Type(TransactionType::REGULAR);

	auto trans = builder.Build();
	MyBlockchain->AddTransaction(trans);
	return trans;
}
