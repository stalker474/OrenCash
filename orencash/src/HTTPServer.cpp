#include "HTTPServer.h"
#include <boost\bind.hpp>
#include "Operator.h"

using namespace orencash;

HTTPServer::HTTPServer(std::shared_ptr<Blockchain> MyBlockchain, unsigned short Port) : Port(Port),MyBlockchain(MyBlockchain)
{
	IsRunning = false;
	MyMiner = nullptr;

	auto resource = std::make_shared< Resource >();
	resource->set_path("/blocks");
	
	resource->set_method_handler("GET", boost::bind(&HTTPServer::GetBlocks_method_handler, this, _1));
	
	auto resource2 = std::make_shared< Resource >();
	resource2->set_path("/transactions");
	resource2->set_method_handler("GET", boost::bind(&HTTPServer::GetTransactions_method_handler, this, _1));

	auto resource3 = std::make_shared< Resource >();
	resource3->set_path("/miner/status");
	resource3->set_method_handler("GET", boost::bind(&HTTPServer::GetMinerStatus_method_handler, this, _1));

	auto resource4 = std::make_shared< Resource >();
	resource4->set_path("/miner/start");
	resource4->set_method_handler("GET", boost::bind(&HTTPServer::GetMinerStart_method_handler, this, _1));

	auto resource5 = std::make_shared< Resource >();
	resource5->set_path("/miner/stop");
	resource5->set_method_handler("GET", boost::bind(&HTTPServer::GetMinerStop_method_handler, this, _1));

	auto resource6 = std::make_shared< Resource >();
	resource6->set_path("/wallet/generate");
	resource6->set_method_handler("GET", boost::bind(&HTTPServer::GetGenerateAddress_method_handler, this, _1));

	auto resource7 = std::make_shared< Resource >();
	resource7->set_path("/wallet/createtransaction");
	resource7->set_method_handler("GET", boost::bind(&HTTPServer::GetCreateTransaction_method_handler, this, _1));

	auto resource8 = std::make_shared< Resource >();
	resource8->set_path("/wallet/checkbalance");
	resource8->set_method_handler("GET", boost::bind(&HTTPServer::GetCheckBalance_method_handler, this, _1));

	MyService.publish(resource);
	MyService.publish(resource2);
	MyService.publish(resource3);
	MyService.publish(resource4);
	MyService.publish(resource5);
	MyService.publish(resource6);
	MyService.publish(resource7);
	MyService.publish(resource8);
	
	MySettings = std::make_shared< Settings >();
	MySettings->set_default_header("Access-Control-Allow-Origin", "*");
	MySettings->set_port(Port);
	MySettings->set_bind_address("127.0.0.1");
	MySettings->set_default_header("Connection", "close");
}


HTTPServer::~HTTPServer()
{
	Stop();
}

void HTTPServer::SetMiner(std::shared_ptr<Miner> NewMiner)
{
	MyMiner = NewMiner;
}

void HTTPServer::Start()
{
	if(!IsRunning)
	{
		IsRunning = true;
		MyThread = std::make_unique<std::thread>([&]()
		{
			MyService.start(MySettings);
		});
	}
}

void HTTPServer::Stop()
{
	if(IsRunning)
		MyService.stop();
	IsRunning = false;
}

void HTTPServer::GetBlocks_method_handler(const std::shared_ptr< Session > session)
{
	std::string value = "{\"blocks\":[";

	std::vector<std::shared_ptr<Block>> blocks;

	if (session->get_request()->has_query_parameter("id"))
	{
		uint64_t nul = 0;
		uint64_t id = session->get_request()->get_query_parameter("id", nul);
		try
		{
			blocks.push_back(MyBlockchain->GetBlockByIndex(id));
		}
		catch (std::exception&) {}
	}
	else if (session->get_request()->has_query_parameter("hash"))
	{
		std::string nul;
		std::string hash = session->get_request()->get_query_parameter("hash", nul);
		try
		{
			auto block = MyBlockchain->GetBlockByHash(hash);
			blocks.push_back(block);
		}
		catch (std::exception&) {}
	}
	else
		blocks = MyBlockchain->GetAllBlocks();

	for (long long i = blocks.size() - 1; i >= 0; --i)
	{
		const auto& block = blocks[i];

		if (i != blocks.size() - 1)
			value += ",";
		value += "{";
		value += "\"index\": \"" +  std::to_string(block->GetIndex()) + "\"";
		value += ",\"hash\": \"" + block->GetHash() + "\"";
		value += ",\"previousHash\": \"" + block->GetPreviousHash() + "\"";
		value += ",\"nonce\": \"" + std::to_string(block->GetNonce()) + "\"";
		value += ",\"time\": \"" + std::to_string(block->GetTimeStamp()) + "\"";
		value += ",\"transactions\":[";
		for (const auto& transact : block->GetTransactions())
		{
			if (transact.GetID() != block->GetTransactions()[0].GetID())
				value += ",";
			value += "{";
			value += "\"id\": \"" + std::to_string(transact.GetID()) + "\"";
			value += ",\"type\": \"";
			switch (transact.GetType())
			{
			case TransactionType::FEE:
				value += "FEE";
				break;
			case TransactionType::REGULAR:
				value += "REGULAR";
				break;
			case TransactionType::REWARD:
				value += "REWARD";
				break;
			}
			value += "\"";
			value += ",\"hash\": \"" + transact.GetHash() + "\"";
			value += ",\"inputs\":[";
			bool first = true;
			for (const auto& input : transact.GetData().Inputs)
			{
				if (first)
					first = false;
				else
					value += ",";

				value += "{";
				value += "\"index\": \"" + std::to_string(input.Index) + "\"";
				value += ",\"transactionID\": \"" + std::to_string(input.TransactionID) + "\"";
				value += ",\"address\": \"" + input.Address + "\"";
				value += ",\"amount\": \"" + std::to_string(input.Amount) + "\"";
				value += ",\"signature\": \"" + input.Signature + "\"";
				value += "}";
			}
			value += "]";
			value += ",\"outputs\":[";
			first = true;
			for (const auto& output : transact.GetData().Outputs)
			{
				if (first)
					first = false;
				else
					value += ",";
				value += "{";
				value += "\"index\": \"" + std::to_string(output.Index) + "\"";
				value += ",\"transactionID\": \"" + std::to_string(output.TransactionID) + "\"";
				value += ",\"address\": \"" + output.Address + "\"";
				value += ",\"amount\": \"" + std::to_string(output.Amount) + "\"";
				value += "}";
			}
			value += "]}";
		}
		value += "]";
		value += "}";
	}
	value += "]";
	value += "}";

	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetTransactions_method_handler(const std::shared_ptr< Session > session)
{
	std::string value = "";
	uint64_t nul = 0;
	TransactionPtrList transactions;

	if(session->get_request()->has_query_parameter("id") && session->get_request()->has_query_parameter("blockid"))
	{
		uint64_t id = session->get_request()->get_query_parameter("id", nul);
		uint64_t blockid = session->get_request()->get_query_parameter("blockid", nul);
		try
		{
			auto block = MyBlockchain->GetBlockByIndex(blockid);
			transactions.push_back(std::make_shared<Transaction>(block->GetTransactionById(id)));
		}
		catch(const std::exception&){}
	}
	else if (session->get_request()->has_query_parameter("id"))
	{
		//get only one transaction
		try
		{
			uint64_t id = session->get_request()->get_query_parameter("id", nul);
			transactions.push_back(MyBlockchain->GetTransactionById(id));
		}
		catch(const std::exception&){}
	}
	else
	{
		transactions = MyBlockchain->GetAllTransactions();
	}

	value = "{\"transactions\" : [";
	
	for (const auto& transact : transactions)
	{
		if (transact.get() != transactions.front().get())
			value += ",";
		value += "{";
		value += "\"id\": \"" + std::to_string(transact->GetID()) + "\"";
		value += ",\"type\": \"";
		switch (transact->GetType())
		{
		case TransactionType::FEE:
			value += "FEE";
			break;
		case TransactionType::REGULAR:
			value += "REGULAR";
			break;
		case TransactionType::REWARD:
			value += "REWARD";
			break;
		}
		value += "\"";
		value += ",\"hash\": \"" + transact->GetHash() + "\"";
		value += ",\"inputs\":[";
		bool first = true;
		for (const auto& input : transact->GetData().Inputs)
		{
			if (first)
				first = false;
			else
				value += ",";
				
			value += "{";
			value += "\"index\": \"" + std::to_string(input.Index) + "\"";
			value += ",\"transactionID\": \"" + std::to_string(input.TransactionID) + "\"";
			value += ",\"address\": \"" + input.Address + "\"";
			value += ",\"amount\": \"" + std::to_string(input.Amount) + "\"";
			value += ",\"signature\": \"" + input.Signature + "\"";
			value += "}";
		}
		value += "]";
		value += ",\"outputs\":[";
		first = true;
		for (const auto& output : transact->GetData().Outputs)
		{
			if (first)
				first = false;
			else
				value += ",";
			value += "{";
			value += "\"index\": \"" + std::to_string(output.Index) + "\"";
			value += ",\"transactionID\": \"" + std::to_string(output.TransactionID) + "\"";
			value += ",\"address\": \"" + output.Address + "\"";
			value += ",\"amount\": \"" + std::to_string(output.Amount) + "\"";
			value += "}";
		}
		value += "]}";
	}
	value += "]";
	value += "}";

	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetMinerStatus_method_handler(const std::shared_ptr<Session> session)
{
	std::string value = "";
	if (MyMiner != nullptr)
	{
		if (MyMiner->IsMining())
			value = "{ \"Status\" : \"Mining\" }";
		else
			value = "{ \"Status\" : \"Stopped\" }";
	}
	else
		value = "{ \"error\" : \"No miner available\" }";;
	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetMinerStart_method_handler(const std::shared_ptr< Session > session)
{
	std::string value = "";
	
	if (MyMiner != nullptr)
	{
		if (session->get_request()->has_query_parameter("addr"))
		{
			std::string str = "";
			str = session->get_request()->get_query_parameter("addr", str);
			MyMiner->Start(str);
			value = "{ \"Status\" : \"Mining\" }";
		}
		else
			value = "{ \"error\" : \"Mining address required\" }";
	}
	else
		value = "{ \"error\" : \"No miner available\" }";;

	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetMinerStop_method_handler(const std::shared_ptr< Session > session)
{
	std::string value = "";
	if (MyMiner != nullptr)
	{
		MyMiner->Stop();
		value = "{ \"Status\" : \"Stopped\" }";
	}
	else
		value = "{ \"error\" : \"No miner available\" }";;
	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetGenerateAddress_method_handler(const std::shared_ptr<Session> session)
{
	std::string value = "";
	Operator op(MyBlockchain);
	auto wlt = op.CreateWalletFromPassword(std::to_string(CryptoHelp::GenerateRandomID()));
	std::string publicKey = wlt->GenerateAddress();
	std::string privateKey = wlt->GetSecretKeyByAddress(publicKey);
	value = "{\"publicKey\":\"";
	value += publicKey;
	value += "\",";
	value += "\"privateKey\":\"";
	value += privateKey;
	value += "\"";
	value += "}";
	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetCreateTransaction_method_handler(const std::shared_ptr<Session> session)
{
	std::string value = "";
	if (session->get_request()->has_query_parameter("from")
		&& session->get_request()->has_query_parameter("privateKey")
		&& session->get_request()->has_query_parameter("to")
		&& session->get_request()->has_query_parameter("amount")
		&& session->get_request()->has_query_parameter("change")
		&& session->get_request()->has_query_parameter("fee"))
	{
		Operator op(MyBlockchain);
		std::string str = "";
		uint64_t zero = 0;
		std::string from = session->get_request()->get_query_parameter("from", str);
		std::string privateKey = session->get_request()->get_query_parameter("privateKey", str);
		std::string to = session->get_request()->get_query_parameter("to", str);
		uint64_t amount = session->get_request()->get_query_parameter("amount", zero);
		uint64_t fee = session->get_request()->get_query_parameter("fee", zero);
		std::string change = session->get_request()->get_query_parameter("change", str);

		if(from.size() && privateKey.size() && to.size() && (amount > 0) && change.size())
		{
			try{
				op.CreateTransaction(privateKey, from, to, amount, fee, change);
				value = "{\"result\":\"OK\"}";
			}
			catch (const std::exception& e)
			{
				value = "{\"result\" : \"";
				value += e.what();
				value += "\"}";
			}
		}
		else 
			value = "{\"result\" : \"invalid parameters\"}";
		
	}
	else
		value = "{\"result\" : \"from,privateKey,to,amount,fee,change required\"}";
	
	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}

void HTTPServer::GetCheckBalance_method_handler(const std::shared_ptr<Session> session)
{
	std::string value = "";
	if (session->get_request()->has_query_parameter("addr"))
	{
		Operator op(MyBlockchain);
		std::string str = "";
		std::string addr = session->get_request()->get_query_parameter("addr", str);

		if (addr.size())
		{
			try {
				value = "{ \"balance\" : \"";
				value += std::to_string(op.GetBalanceForWalletAddress(addr));
				value += "\"}";
			}
			catch (const std::exception& e)
			{
				value = "{\"error\" : \"";
				value += e.what();
				value += "\"}";
			}
		}
		else
			value = "{\"error\" : \"invalid parameters\"}";

	}
	else
		value = "{\"error\" : \"address required\"}";

	session->close(OK, value, { { "Content-Length", std::to_string(value.size()) } });
}
