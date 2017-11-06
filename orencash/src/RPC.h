#ifndef RPC_H
#define RPC_H

#include <iostream>
#include <sstream>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <rpc/client.h>
#include <rpc/server.h>

#include "Blockchain.h"

#define MAX_PEERS 8

namespace orencash
{
	struct PeerAddress
	{
		std::string host;
		unsigned short port;
		mutable time_t	last_seen_timestamp;
		mutable bool temporary_banned; //this will not be serialized

		PeerAddress() { temporary_banned = false; }

		PeerAddress(std::string _host, unsigned short _port, time_t	_last_seen_timestamp)
		{
			host = _host;
			port = _port;
			last_seen_timestamp = _last_seen_timestamp;
			temporary_banned = false;
		}

		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & host;
			archive & port;
			archive & last_seen_timestamp;
		}

		bool operator==(const PeerAddress& elem) const
		{
			return (elem.host == host) && (elem.port == port);
		}

		bool operator!=(const PeerAddress& elem) const
		{
			return !(elem == *this);
		}

		bool operator<(const PeerAddress& elem) const
		{
			std::string str1 = host + std::to_string(port);
			std::string str2 = elem.host + std::to_string(elem.port);
			return str1 < str2;
		}
	};

	class PeerList
	{
	public:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & archive, const unsigned int file_version)
		{
			archive & MyPeers;
		}

		std::string Serialize() const
		{
			std::ostringstream oss;
			{
				boost::archive::text_oarchive oa(oss);
				oa << *this;
			}
			return oss.str();
		}
		std::vector<PeerAddress> MyPeers;
	};

    class NetworkListenerInterface
	{
	public:
		virtual void OnReceivedBlockchain(const std::vector<Block>& Blocks) = 0;
		virtual void OnReceivedLastBlock(const Block& Blocks) = 0;
		virtual void OnReceivedTransactions(const std::vector<Transaction>& Transactions) = 0;
		virtual void OnConnected() = 0;
	};

	class P2PServer
	{
	public:
		P2PServer(class Node * MyNode, unsigned short Port, const std::string& publicAddr);
		~P2PServer();

		void							Run();
		void							Shutdown();
		void							JoinKnownPeers();
		void							AddPeer(PeerAddress NewAddress);

		//listeners
		void							Subscribe(NetworkListenerInterface* NewListener);
		void							Unsubscribe(NetworkListenerInterface* NewListener);

		
		void							RequestLatestBlock();
		void							RequestTransactions();
		void							RequestBlockchain();

		void							SubmitBlock(const std::shared_ptr<Block>& NewBlock);
		void							SubmitTransaction(const std::shared_ptr<Transaction>& NewTransaction);


		//wrapped RPC calls
		const BlockPtrList				GetBlockchain();
		const TransactionPtrList		GetTransactions();
		const std::shared_ptr<Block>	GetLatestBlock();
		void							ReceiveBlock(const Block& NewBlock);
		void							ReceiveTransaction(const Transaction & NewTransaction);
		bool							AcceptPeer(const PeerAddress& Peer);
		
	private:
		std::shared_ptr<rpc::server>				MyServer;
		std::list<std::shared_ptr<rpc::client>>		Clients;
		std::list<NetworkListenerInterface*>		Listeners;
		unsigned short								MyPort;
		std::string									PublicAddr;
		std::mutex									PeersListMutex;
		std::list<PeerAddress>						Addresses;
		class Node *								MyNode;
	};
}


#endif

