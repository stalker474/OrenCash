#include "Database.h"

#ifdef WITH_ODB
#include <odb/database.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/schema-catalog.hxx>
#include "PersistentObjects.h"
#include "PersistentObjects-odb.hxx"
#endif

#include <sstream>

using namespace orencash;

bool Database::Connect(const std::string& User,const std::string& Password) noexcept
{
#ifdef WITH_ODB
	ConnectionObject = std::make_unique<odb::sqlite::database>(Name, SQLITE_OPEN_READWRITE, false);
	
	//load blockchain from DB
	typedef odb::result<persist::Block> result;
	odb::transaction t(ConnectionObject->begin());
	result r(ConnectionObject->query<persist::Block>());
	for (const persist::Block& b : r)
	{
		//for now we keep using RAM storage also
		Blocks.emplace_back(std::make_shared<Block>(b.Index, b.PreviousHash, b.TimeStamp, b.Nonce, b.Hash));
		TransactionList transactions;

		for (const auto& trans : b.Transactions)
		{
			transactions.emplace_back(trans->ID, trans->Hash, (TransactionType)trans->Type);

			for (const auto& input : trans->Inputs)
				transactions.back().AddInputs({
				{
				input.TransactionID,
				input.Index,
				input.Amount,
				input.Address,
				input.Signature }});

			for (const auto& output : trans->Outputs)
				transactions.back().AddOutputs({ 
				{
				output.TransactionID,
				output.Index,
				output.Amount,
				output.Address} });
		}
		Blocks.back()->AddTransactions(transactions);
	}
	t.commit();

	//load wallets from DB
	typedef odb::result<persist::Wallet> resultWallets;
	odb::transaction t2(ConnectionObject->begin());
	resultWallets rW(ConnectionObject->query<persist::Wallet>());
	for (const persist::Wallet& w : rW)
	{
		Wallets.emplace_back(std::make_shared<Wallet>(w.ID, w.PasswordHash, w.Secret));
		for (const auto& pair : w.Keypairs)
			Wallets.back()->AddKeypair({pair.PublicKey,pair.PrivateKey});
	}
	t2.commit();
	
	return ConnectionObject != nullptr;
#else
	return true;
#endif
}

bool Database::Disconnect()
{
	return true;
}
#ifdef WITH_ODB
void Database::Persist(const Block & NewBlock)
{
	orencash::persist::Block block;
	block.Index = NewBlock.GetIndex();
	block.Hash = NewBlock.GetHash();
	block.PreviousHash = NewBlock.GetPreviousHash();
	block.Nonce = NewBlock.GetNonce();
	block.TimeStamp = NewBlock.GetTimeStamp();
	for (const Transaction& transaction : NewBlock.GetTransactions())
	{
		auto trans = std::make_shared<orencash::persist::Transaction>();
		trans->ID = transaction.GetID();
		trans->Hash = transaction.GetHash();
		switch (transaction.GetType())
		{
		case TransactionType::REWARD:
			trans->Type = persist::REWARD;
			break;
		case TransactionType::FEE:
			trans->Type = persist::FEE;
			break;
		case TransactionType::REGULAR:
			trans->Type = persist::REGULAR;
			break;
		}

		for (const TransactionInput& input : transaction.GetData().Inputs)
			trans->Inputs.push_back({
			input.TransactionID,
			input.Index,
			input.Amount,
			input.Address,
			input.Signature });

		for (const TransactionOutput& output : transaction.GetData().Outputs)
			trans->Outputs.push_back({
			output.TransactionID,
			output.Index,
			output.Amount,
			output.Address
		});

		block.Transactions.push_back(trans);
		ConnectionObject->persist(*trans);
	}

	ConnectionObject->persist(block);
}

void Database::Persist(const Wallet & NewWallet)
{
	orencash::persist::Wallet wallet;
	wallet.ID = NewWallet.GetID();
	wallet.PasswordHash = NewWallet.GetPasswordHash();
	wallet.Secret = NewWallet.GetSecret();
	
	for (const auto& pair : NewWallet.GetKeypairs())
		wallet.Keypairs.push_back({pair.first,pair.second});

	ConnectionObject->persist(wallet);
}
#endif
void Database::AddBlock(const Block & NewBlock)
{
	//for now we keep using RAM storage also
	Blocks.emplace_back(std::make_shared<Block>(NewBlock.GetIndex(), NewBlock.GetPreviousHash(), NewBlock.GetTimeStamp(), NewBlock.GetNonce(), NewBlock.GetHash()));
	Blocks.back()->AddTransactions(NewBlock.GetTransactions());

#ifdef WITH_ODB
	try
	{
		odb::transaction t(ConnectionObject->begin());
		Persist(NewBlock);
		t.commit();
	}
	catch (const odb::exception& e)
	{
        throw DatabaseException(e.what());
	}
#endif
}

void Database::AddTransaction(const Transaction & NewTransaction)
{
#ifdef WITH_ODB
#else
#endif
	Transactions.emplace_back(std::make_shared<Transaction>(NewTransaction.GetID(),NewTransaction.GetHash(),NewTransaction.GetType()));
	Transactions.back()->AddInputs(NewTransaction.GetData().Inputs);
	Transactions.back()->AddOutputs(NewTransaction.GetData().Outputs);
}

void Database::AddWallet(const Wallet & NewWallet)
{
	Wallets.emplace_back(std::make_shared<Wallet>(NewWallet.GetID(), NewWallet.GetPasswordHash(), NewWallet.GetSecret()));
	Wallets.back()->GetKeypairs().insert(Wallets.back()->GetKeypairs().begin(),NewWallet.GetKeypairs().begin(), NewWallet.GetKeypairs().end());

#ifdef WITH_ODB
	try
	{
		odb::transaction t(ConnectionObject->begin());
		Persist(NewWallet);
		t.commit();
	}
	catch (const odb::exception& e)
	{
        throw DatabaseException(e.what());
	}
#endif
}

void Database::UpdateWallet(const Wallet & TheWallet)
{
	assert(false);
}

const std::vector<AddressPair> Database::GetAddressesForWallet(const uint64_t & ID)
{
	auto res = std::find_if(Wallets.begin(), Wallets.end(), 
		[ID](const auto& wallet)
	{
		return wallet->GetID() == ID;
	}
	);
	if (res == Wallets.end())
		throw DatabaseException("Wallet id not found");
	return res[0]->GetKeypairs();
}

void Database::RemoveTransactionByID(uint64_t ID)
{
	auto res = std::find_if(Transactions.begin(),Transactions.end(),
		[ID](const auto& elem)
	{
		return elem->GetID() == ID;
	});

	if (res != Transactions.end())
		Transactions.erase(res);
}

const std::shared_ptr<Block> Database::GetBlockByIndex(uint64_t Index) const
{
	auto res = std::find_if(Blocks.begin(),Blocks.end(),
		[Index](const auto& elem)
	{
		return elem->GetIndex() == Index;
	});
	if (res == Blocks.end())
		throw DatabaseException("Block index not found");

	return res[0];
}

const std::shared_ptr<Block> Database::GetBlockByHash(std::string Hash) const
{
	auto res = std::find_if(Blocks.begin(), Blocks.end(),
		[Hash](const auto& elem)
	{
		return elem->GetHash() == Hash;
	});
	if (res == Blocks.end())
		throw DatabaseException("Block Hash not found");

	return res[0];
}

const std::shared_ptr<Block> Database::GetLastBlock() const
{
	if (Blocks.size() == 0)
		return nullptr;
	return Blocks.back();
}

const std::shared_ptr<Transaction> Database::GetTransactionById(const uint64_t & ID) const
{
	auto res = std::find_if(Transactions.begin(),Transactions.end(),
		[ID](const auto& elem)
	{
		return elem->GetID() == ID;
	});

	if (res != Transactions.end())
		return *res;
	else
		throw DatabaseException("Transaction id not found");
}

const std::shared_ptr<Wallet> Database::GetWalletById(const uint64_t & ID)
{
	auto res = std::find_if(Wallets.begin(),Wallets.end(),
		[ID](const auto& elem)
	{
		return elem->GetID() == ID;
	});
	if (res == Wallets.end())
		throw DatabaseException("Wallet id not found");
	return res[0];
}

void Database::ValidateConnection()
{
}
