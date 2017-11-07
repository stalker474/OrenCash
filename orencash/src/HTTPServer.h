#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <restbed>
#include <memory>
#include <cstdlib>
#include <iostream>
#include "Blockchain.h"
#include "Miner.h"

using namespace restbed;

namespace orencash
{
	class HTTPServer
	{
	public:
		HTTPServer(std::shared_ptr<Blockchain> MyBlockchain, unsigned short Port);
		~HTTPServer();

		void SetMiner(std::shared_ptr<Miner> NewMiner);

		void Start();
		void Stop();
	private:
		void GetMinerStatus_method_handler(const std::shared_ptr< Session > session);
		void GetBlocks_method_handler(const std::shared_ptr< Session > session);
		void GetTransactions_method_handler(const std::shared_ptr< Session > session);
		void GetMinerStart_method_handler(const std::shared_ptr< Session > session);
		void GetMinerStop_method_handler(const std::shared_ptr< Session > session);
		void GetGenerateAddress_method_handler(const std::shared_ptr< Session > session);
		void GetCreateTransaction_method_handler(const std::shared_ptr< Session > session);
		void GetCheckBalance_method_handler(const std::shared_ptr< Session > session);


		Service							MyService;
		std::unique_ptr<std::thread>	MyThread;
		std::shared_ptr<Blockchain>		MyBlockchain;
		std::shared_ptr<Settings>		MySettings;
		std::shared_ptr<Miner>			MyMiner;
		unsigned short					Port;
		bool							IsRunning;
	};
}

#endif

