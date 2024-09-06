#pragma once

#include <network/ServerInterface.hpp>

#include "server_handlers.hpp"

class DopechainTracker : public Net::ServerInterface<DopechainMessage> {
private:
	Net::HandlerMediator<DopechainMessage, DopechainTypeMessage> mediator;
	std::shared_ptr<DopechainContainerPeers> container = std::make_shared<DopechainContainerPeers>();

public:	
	DopechainTracker(const uint16_t& _port, const DWORD& _countThread) : Net::ServerInterface<DopechainMessage>{ _port }, mediator{ _countThread } {
		mediator.RegisterHandler(DopechainTypeMessage::SERVER_PING, std::make_unique<ServerPing>());
		mediator.RegisterHandler(DopechainTypeMessage::PEERS_LIST, std::make_unique<PeersList>(container));
		mediator.RegisterHandler(DopechainTypeMessage::REGISTRATION_PEER, std::make_unique<RegistrationPeer>(container));
	}
	 
	~DopechainTracker() = default;
private:
	void OnMessage(std::shared_ptr<Net::OwnerMessage<DopechainMessage>> _ownMsg) {
		mediator.HandleMessage(_ownMsg);
	}

	void OnConnect(std::shared_ptr<Net::Connection<DopechainMessage>> _handleClient) {
		spdlog::info("Client connection: {0}:{1}", _handleClient->GetAddressRemote(), _handleClient->GetPortRemote());
	}

	void OnDisconnect(std::shared_ptr<Net::Connection<DopechainMessage>> _handleClient) {
		spdlog::info("Client Disconnect");
	}
};