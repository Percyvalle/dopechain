#pragma once

#include <network/ClientInterface.hpp>

#include <message_impl.hpp>

class DopechainPeer : public Net::ClientInterface<DopechainMessage> {
private:
	DopechainPeerInfo info;

public:
	DopechainPeer() = default;
	DopechainPeer(DopechainPeerInfo& _peerInfo) {
		info = _peerInfo;
	}

	void Connect() {
		Net::ClientInterface<DopechainMessage>::Connect(info.address, info.port);
	}

	void Ping() {
		DopechainMessage message = MessageFactory::Create(DopechainTypeMessage::SERVER_PING, DopechainStatusMessage::CLIENT_STATUS);

		Send(message);
	}

	Net::OWNER_MESSAGE<DopechainMessage> BlockchainVersion() {
		DopechainMessage message = MessageFactory::Create(DopechainTypeMessage::BLOCKCHAIN_VERSION, DopechainStatusMessage::CLIENT_STATUS);

		Send(message);

		return WaitingResponse();
	}

	Net::OWNER_MESSAGE<DopechainMessage> BlockhcainSync() {
		DopechainMessage message = MessageFactory::Create(DopechainTypeMessage::BLOCKCHAIN_SYNC, DopechainStatusMessage::CLIENT_STATUS);

		Send(message);

		return WaitingResponse();
	}

	Net::OWNER_MESSAGE<DopechainMessage> WaitingResponse() {
		Incoming().wait();
		return !Incoming().empty() ? Incoming().pop_front() : nullptr;
	}

	std::string Address() {
		return info.address;
	}

	std::uint16_t Port() {
		return info.port;
	}
};