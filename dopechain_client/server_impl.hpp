#pragma once

#include <network/ServerInterface.hpp>

#include <message_impl.hpp>

#include "server_impl_handlers.hpp"

class DopechainServer : public Net::ServerInterface<DopechainMessage> {
private:
	Net::HandlerMediator<DopechainMessage, DopechainTypeMessage> mediator;

public:
	DopechainServer(const uint16_t& _port, const DWORD& _countThread, std::shared_ptr<DopechainBlockchain> _blockchain) : Net::ServerInterface<DopechainMessage>{ _port }, mediator{ _countThread } {
		mediator.RegisterHandler(DopechainTypeMessage::SERVER_PING, std::make_unique<Ping>());
		mediator.RegisterHandler(DopechainTypeMessage::BLOCKCHAIN_SYNC, std::make_unique<BlockchainSync>(_blockchain));
		mediator.RegisterHandler(DopechainTypeMessage::BLOCKCHAIN_VERSION, std::make_unique<BlockchainVersion>(_blockchain));
	}

private:
	void OnMessage(std::shared_ptr<Net::OwnerMessage<DopechainMessage>> _ownMsg) {
		spdlog::info("Message: {0}:{1}", _ownMsg->Message().StringStatus(), _ownMsg->Message().StringType());
		mediator.HandleMessage(_ownMsg);
	};

	void OnConnect(std::shared_ptr<Net::Connection<DopechainMessage>> _handleClient) {
		spdlog::info("Connection: {}:{}", _handleClient->GetAddressRemote(), _handleClient->GetPortRemote());
	};

	void OnDisconnect(std::shared_ptr<Net::Connection<DopechainMessage>> _handleClient) {
		spdlog::info("Disconnect: {}:{}", _handleClient->GetAddressRemote(), _handleClient->GetPortRemote());
	};
};