#include "Blockchain.h"
#include "Operator.h"
#include "Miner.h"
#include "P2P.h"
#include "Node.h"
#include "HTTPServer.h"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace orencash;

static shared_ptr<Blockchain> bs = make_shared<Blockchain>();

void Info()
{
	std::cout << "Usage :" << std::endl;
	std::cout << "orencash.exe -node -port {nodeport} -addr {publicaddr} [-friend {friendaddr}]" << std::endl;
	std::cout << "orencash.exe -miner -port {nodeport} -addr {publicaddr} [-friend {friendaddr}] -hport {httpport}" << std::endl;
	std::cout << "orencash.exe -http -port {nodeport} -addr {publicaddr}  [-friend {friendaddr}] -hport {httpport}" << std::endl;
}

int readParams(int argc, char *argv[],int& mode, int& myPort, std::string& publicAddr, int& httpPort, std::string& friendAddr)
{
	for (size_t i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-node") == 0)
		{
			if (mode == -1)
				mode = 0;
			else
				return -1;
		}
		else if (strcmp(argv[i], "-miner") == 0)
		{
			if (mode == -1)
				mode = 1;
			else
				return -1;
		}
		else if (strcmp(argv[i], "-http") == 0)
		{
			if (mode == -1)
				mode = 2;
			else
				return -1;
		}
		else if (strcmp(argv[i], "-port") == 0)
		{
			if (i + 1 < argc)
			{
				try
				{
					myPort = atoi(argv[i + 1]);
					++i;
				}
				catch (...)
				{
					return -1;
				}
			}
			else
				return -1;
		}
		else if (strcmp(argv[i], "-hport") == 0)
		{
			if (i + 1 < argc)
			{
				try
				{
					httpPort = atoi(argv[i + 1]);
					++i;
				}
				catch (...)
				{
					return -1;
				}
			}
			else
				return -1;
		}
		else if (strcmp(argv[i], "-friend") == 0)
		{
			if (i + 1 < argc)
			{
				friendAddr = argv[i + 1];
				++i;
			}
			else
				return -1;
		}
		else if (strcmp(argv[i], "-addr") == 0)
		{
			if (i + 1 < argc)
			{
				publicAddr = argv[i + 1];
				++i;
			}
			else
				return -1;
		}
		else
			return -1;
	}

	return 1;
}

#define CRITICAL(x) if(!(x)) { Info(); return -1;}

int main(int argc, char *argv[])
{
	//uint64_t balance1 = op.GetBalanceForWalletAddress(wallet->GetID(), addr1);
	//uint64_t balance2 = op.GetBalanceForWalletAddress(wallet->GetID(), addr2);
	/*mi.Mine(minerAddr);
	balance1 = op.GetBalanceForWalletAddress(wallet->GetID(),addr1);
	balance2 = op.GetBalanceForWalletAddress(wallet->GetID(), addr2);
	uint64_t balance3 = op.GetBalanceForWalletAddress(wallet->GetID(), minerAddr);
	*/

	int mode = -1;
	int myPort = 0;
	int httpPort = 0;
	std::string friendAddr = "";
	std::string publicAddr = "";
	int friendPort = -1;
	std::string friendHost = "";

	CRITICAL(readParams(argc, argv, mode, myPort, publicAddr, httpPort, friendAddr));
	CRITICAL(myPort > 0)

	if(friendAddr.size())
	{
		std::vector<std::string> strs;
		boost::split(strs, friendAddr, boost::is_any_of(":"));
		CRITICAL(strs.size() == 2);

		friendHost = strs[0];
		try
		{
			friendPort = atoi(strs[1].c_str());
		}
		catch (...)
		{
			Info();
			return -1;
		}
	}

	CRITICAL(mode >= 0 && myPort > 0);

	if(mode == 0)
	{
		Operator op(bs);
		Node node(bs, myPort, publicAddr);
		if (friendHost.size() && friendPort > 0)
			node.ConnectToPeer({friendHost,static_cast<unsigned short>(friendPort), 0});
		node.Start();
	}
	else if(mode == 2)
	{
		Node node(bs, myPort, publicAddr);
		if (friendHost.size() && friendPort > 0)
			node.ConnectToPeer({ friendHost,static_cast<unsigned short>(friendPort), 0 });
		CRITICAL(httpPort > 0);
		HTTPServer server(bs,httpPort);
		server.Start();
		node.Start();
		server.Stop();
	}
	else if(mode == 1)
	{
		Node node(bs, myPort, publicAddr);
		if (friendHost.size() && friendPort > 0)
			node.ConnectToPeer({ friendHost,static_cast<unsigned short>(friendPort), 0 });
		CRITICAL(httpPort > 0);
		HTTPServer server(bs, httpPort);
		server.SetMiner(std::make_shared<Miner>(bs));
		server.Start();
		node.Start();
		server.Stop();
	}
	else
	{
		Info();
		return -1;
	}

    return 0;
}

