#pragma once

#include "client_impl.hpp"
#include "server_impl.hpp"
#include "blockchain.hpp"
#include "peer_manager.hpp"

#include <application/CommadLineParser.hpp>
#include <application/InteractiveInput.hpp>
#include <application/ApplicationInteface.hpp>

#define LOCAL_SERVER_PORT optionsParser.Get<std::uint16_t>("--client-port")
#define SERVER_TRACKER_PORT optionsParser.Get<std::uint16_t>("--server-port")
#define SERVER_TRACKER_ADDRESS optionsParser.Get<std::string>("--server-address")
#define DEBUG_MODE_ENABLED optionsParser.Get<bool>("--debug")
#define LOGICAL_PROCESSORS_SIZE Utils::GetCountCPU()

class DopechainApp : public IApp {
private:
	std::shared_ptr<DopechainClient> clientTrackerServer;
	std::shared_ptr<DopechainServer> serverLocalPeer;
	std::shared_ptr<DopechainBlockchain> blockchainLogic;
	std::shared_ptr<DopechainPeerManager> peerManager;
	std::shared_ptr<DopechainContainerPeers> peerContainer;

	std::atomic_bool application_flag = true;

	InteractiveInput inputParser;
	CommadLineParser optionsParser; 

	std::thread asyncProcessHandleThread;

	void AsyncProcessHandle() {
		while (serverLocalPeer->IsRunning()) {
			serverLocalPeer->Update(false);
			serverLocalPeer->CheckClientConnection();
		}
	}

	void StartAsyncProcessHandle() {
		asyncProcessHandleThread = std::thread(&DopechainApp::AsyncProcessHandle, this);
	}

public:
	DopechainApp(int argc, char** argv) : optionsParser{argc, argv} {
		optionsParser.AddOptions<std::string>("--server-address", "-sa", "Server tracker address");
		optionsParser.AddOptions<std::uint16_t>("--server-port", "-sp", "Server tracker port");
		optionsParser.AddOptions<std::uint16_t>("--client-port", "-cp", "Client port");
		optionsParser.AddOptions<bool>("--debug", "-d", "On debug output", true);

		optionsParser.Parse();
	}

	~DopechainApp() {
		serverLocalPeer->Stop();
		clientTrackerServer->Disconnect();

		if (asyncProcessHandleThread.joinable()) {
			asyncProcessHandleThread.join();
		}
	}

	bool ConnectionsTrackerServer() {
		clientTrackerServer->Connect(SERVER_TRACKER_ADDRESS, SERVER_TRACKER_PORT);
		if (!clientTrackerServer->IsConnected()) {
			spdlog::error("Initialize app: connected server failed!");
			return false;
		}

		Net::OWNER_MESSAGE<DopechainMessage> ownMessagePing = clientTrackerServer->ServerPing();
		if (ownMessagePing == nullptr) {
			spdlog::error("Initialize app: unable to connect to server");
			return false;
		}

		DopechainMessage resposePing = ownMessagePing->Message();
		if (resposePing.Status() != DopechainStatusMessage::SUCCESS) {
			spdlog::error("Initialize app: server is not responding");
			return false;
		}

		return true;
	}

	bool RegistrationTrackerServer() {
		Net::OWNER_MESSAGE<DopechainMessage> ownMessageReg = clientTrackerServer->Registration(serverLocalPeer->Address(), serverLocalPeer->Port(), "danil");
		if (ownMessageReg == nullptr) {
			spdlog::error("Registration peer: unable to connect to server");
			return false;
		}

		DopechainMessage responseReg = ownMessageReg->Message();
		if (responseReg.Status() != DopechainStatusMessage::SUCCESS) {
			spdlog::error("Registration peer: server is not responding");
			return false;
		}
																		
		return true;
	}

	bool InitializeChain() {
		
		bool result = false;

		if (peerContainer->Empty()) {
			result = blockchainLogic->InitBlockchain(DopechainBlockchain::GENESIS);
		}
		else {
			result = blockchainLogic->InitBlockchain(DopechainBlockchain::JOIN);
			result = result && peerManager->BlockchainSync(blockchainLogic);
		}

		return result;
	}

	bool InitializePeers() {
		Net::OWNER_MESSAGE<DopechainMessage> ownMessagePeersList = clientTrackerServer->PeersList();
		if (ownMessagePeersList == nullptr) {
			spdlog::error("Initialize peers: error getting peer list - unable to connect to server");
			return false;
		}

		DopechainMessage responsePeersList = ownMessagePeersList->Message();
		if (responsePeersList.Status() != DopechainStatusMessage::SUCCESS) {
			spdlog::error("Initialize peers: error getting peer list - server not responding");
			return false;
		}

		peerContainer = std::make_shared<DopechainContainerPeers>(responsePeersList.ToJson().template get<DopechainContainerPeers>());
	
		// Removing information about a local peer client from the general peer list
		peerContainer->Erase(clientTrackerServer->Info());

		return true;
	}

	bool InitializeLocalPeerServer() {
		if (!serverLocalPeer->Start()) {
			return false;
		}

		return true;
	}

	bool Initialize() {

		if (DEBUG_MODE_ENABLED) {
			spdlog::set_level(spdlog::level::debug);
		}
		else {
			spdlog::set_level(spdlog::level::info);
		}

		blockchainLogic = std::make_shared<DopechainBlockchain>();
		serverLocalPeer = std::make_shared<DopechainServer>(LOCAL_SERVER_PORT, LOGICAL_PROCESSORS_SIZE, blockchainLogic);
		clientTrackerServer = std::make_shared<DopechainClient>();

		if (!ConnectionsTrackerServer()) {
			return false;
		}
		
		if (!RegistrationTrackerServer()) {
			return false;
		}

		if (!InitializeLocalPeerServer()) {
			return false;
		}																				

		StartAsyncProcessHandle();

		if (!InitializePeers()) {
			return false;
		}

		peerManager = std::make_shared<DopechainPeerManager>(peerContainer);

		if (!InitializeChain()) {
			return false;
		}

		return true;
	}

	void Shutdown() {
		application_flag.store(false);
	}

	bool IsWorking() {
		return application_flag.load();
	}

	void Update() {

		inputParser.Registration("print", &DopechainApp::CallbackPrint, this);
		inputParser.Registration("peers", &DopechainApp::CallbackPeers, this);
		inputParser.Registration("transfer", &DopechainApp::CallbackTransfer, this);

		while (!clientTrackerServer->Incoming().empty()) {
			Net::OWNER_MESSAGE<DopechainMessage> message = clientTrackerServer->Incoming().pop_front();
			spdlog::debug("Message Type {} | Message Status {}", static_cast<int>(message->Message().Type()), static_cast<int>(message->Message().Status()));
		}

		inputParser.Execute();
	}

	void CallbackPrint() {
		blockchainLogic->PrintBlockchain();
	}

	void CallbackPeers() {
		peerContainer->PrintPeers();
	}

	void CallbackTransfer() {
		std::string recipient = inputParser.InputString("recipient: ");
		int amount = inputParser.InputInt("amount: ");
	}
};