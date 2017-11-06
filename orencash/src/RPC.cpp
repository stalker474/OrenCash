#include "RPC.h"
#include <boost/bind.hpp>
#include "Node.h"

using namespace orencash;

template<typename T>
std::string Serialize(T obj)
{
	std::ostringstream oss;
	{
		boost::archive::text_oarchive oa(oss);
		oa << obj;
	}
	return oss.str();
}

template<typename T>
T Deserialize(const std::string& data)
{
	T elem;
	std::istringstream iss(data);
	boost::archive::text_iarchive ia(iss);
	ia & elem;
	return std::move(elem);
}

P2PServer::P2PServer(Node * MyNode,unsigned short Port, const std::string& PublicAddr) 
	: MyPort(Port),
	MyNode(MyNode),
	PublicAddr(PublicAddr)
{
	MyServer = std::make_shared<rpc::server>(MyPort);

	MyServer->bind("AcceptPeer", [&](const std::string data)
	{
		return AcceptPeer(Deserialize<PeerAddress>(data));
	});
	MyServer->bind("GetBlockchain", [&]()
	{
		auto blocksList = GetBlockchain();
		std::vector<Block> blocks;
		blocks.reserve(blocksList.size());
		for (auto& block : blocksList)
			blocks.push_back(*block);
		return Serialize(blocks);
	});
	MyServer->bind("GetLatestBlock", [&]()
	{
		return Serialize(*GetLatestBlock());
	});
	MyServer->bind("GetTransactions", [&]()
	{
		auto transactionsList = GetTransactions();
		std::vector<Transaction> transactions;
		transactions.reserve(transactionsList.size());
		for (auto& transaction : transactionsList)
			transactions.push_back(*transaction);
		return Serialize(transactions);
	});
	MyServer->bind("ReceiveBlock", [&](const std::string data)
	{
		ReceiveBlock(Deserialize<Block>(data));
	});
	MyServer->bind("ReceiveTransaction", [&](const std::string data)
	{
		ReceiveTransaction(Deserialize<Transaction>(data));
	});
}

P2PServer::~P2PServer()
{
	Shutdown();
}

void P2PServer::Run()
{
	MyServer->run();
}

void P2PServer::JoinKnownPeers()
{
	PeersListMutex.lock();
	for (const auto& addr : Addresses)
	{
		Clients.push_back(std::make_shared<rpc::client>(addr.host, addr.port));
        Clients.back()->async_call("AcceptPeer", Serialize(PeerAddress(PublicAddr,MyPort,0)));
	}
	PeersListMutex.unlock();

	for (auto& listener : Listeners)
		listener->OnConnected();
}

void P2PServer::AddPeer(PeerAddress NewAddress)
{
	Addresses.insert(Addresses.end(),NewAddress);
}

void P2PServer::Subscribe(NetworkListenerInterface * NewListener)
{
	Listeners.insert(Listeners.begin(), NewListener);
}

void P2PServer::Unsubscribe(NetworkListenerInterface * NewListener)
{
	Listeners.remove(NewListener);
}

const BlockPtrList P2PServer::GetBlockchain()
{
	return MyNode->GetBlocks();
}

const TransactionPtrList P2PServer::GetTransactions()
{
	return MyNode->GetTransactions();
}

const std::shared_ptr<Block> P2PServer::GetLatestBlock()
{
	return MyNode->GetLastBlock();
}

void P2PServer::Shutdown()
{
	MyServer->stop();
}

void P2PServer::RequestLatestBlock()
{
	std::list<std::future<clmdep_msgpack::v1::object_handle>> results;

	for (const auto& peer : Clients)
		results.insert(results.end(), peer->async_call("GetLatestBlock"));

	for (auto& result : results)
	{
		result.wait();

		for (auto& listener : Listeners)
			listener->OnReceivedLastBlock(
				Deserialize<Block>(result.get().as<std::string>())
			);
	}
}

void P2PServer::RequestTransactions()
{
	std::list<std::future<clmdep_msgpack::v1::object_handle>> results;

	for (const auto& peer : Clients)
		results.insert(results.end(), peer->async_call("GetTransactions"));

	for (auto& result : results)
	{
		result.wait();

		for (auto& listener : Listeners)
			listener->OnReceivedTransactions(
				Deserialize<std::vector<Transaction>>(result.get().as<std::string>())
			);
	}
}

void P2PServer::RequestBlockchain()
{
	std::list<std::future<clmdep_msgpack::v1::object_handle>> results;
	for (const auto& peer : Clients)
		results.insert(results.end(),peer->async_call("GetBlockchain"));

	for (auto& result : results)
	{
		result.wait();

		for (auto& listener : Listeners)
			listener->OnReceivedBlockchain(
				Deserialize<std::vector<Block>>(result.get().as<std::string>())
			);
			
	}
}

void P2PServer::ReceiveBlock(const Block & NewBlock)
{
	for (auto& listener : Listeners)
		listener->OnReceivedLastBlock(NewBlock);
}

void P2PServer::ReceiveTransaction(const Transaction & NewTransaction)
{
	for (auto& listener : Listeners)
		listener->OnReceivedTransactions({ NewTransaction });
}

bool P2PServer::AcceptPeer(const PeerAddress & Peer)
{
	std::cout << "New peer address received : " << Peer.host << ":" << Peer.port << std::endl;
	if (Clients.size() >= MAX_PEERS)
		return false;
	PeersListMutex.lock();
	Clients.push_back(std::make_shared<rpc::client>(Peer.host, Peer.port));
	PeersListMutex.unlock();
	std::cout << "New peer address added : " << Peer.host << ":" << Peer.port << std::endl;
	return true;
}

void P2PServer::SubmitBlock(const std::shared_ptr<Block> & NewBlock)
{
	for (const auto& peer : Clients)
		peer->async_call("ReceiveBlock",NewBlock->Serialize());
}

void P2PServer::SubmitTransaction(const std::shared_ptr<Transaction>& NewTransaction)
{
	for (const auto& peer : Clients)
		peer->async_call("ReceiveTransaction", NewTransaction->Serialize());
}


