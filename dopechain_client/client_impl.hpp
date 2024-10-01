#pragma once

#include <network/ClientInterface.hpp>
#include <network/ServerInterface.hpp>

#include <message_impl.hpp>
#include <container_peers.hpp>

class DopechainClient : public Net::ClientInterface<DopechainMessage> {
private:
	DopechainPeerInfo info;
public:
	const DopechainPeerInfo& Info() {
		return info;
	}

	Net::OWNER_MESSAGE<DopechainMessage> ServerPing() {
		DopechainMessage msg = MessageFactory::Create(DopechainTypeMessage::SERVER_PING, DopechainStatusMessage::CLIENT_STATUS);

		Send(msg);

		return WaitingResponse();
	}

	Net::OWNER_MESSAGE<DopechainMessage> Registration(const std::string& _address, const std::uint16_t& _port, const std::string& _username) {
		info = DopechainPeerInfo(_address, _port, _username);

		DopechainMessage msg = MessageFactory::Create(DopechainTypeMessage::REGISTRATION_PEER, DopechainStatusMessage::CLIENT_STATUS, info);

		Send(msg);

		return WaitingResponse();
	}

	Net::OWNER_MESSAGE<DopechainMessage> PeersList() {
		if (!IsConnected()) {
			return nullptr;
		}

		DopechainMessage msg = MessageFactory::Create(DopechainTypeMessage::PEERS_LIST, DopechainStatusMessage::CLIENT_STATUS);

		Send(msg);

		return WaitingResponse();
	}

	Net::OWNER_MESSAGE<DopechainMessage> WaitingResponse() {
		Incoming().wait(MILISEC_DURATION(3250));
		return !Incoming().empty() ? Incoming().pop_front() : nullptr;
	}
};