#pragma once

#include <network/Handler.hpp>

#include <message_impl.hpp>

#include "container_peers.hpp"

struct ServerPing : Net::MessageHandler<DopechainMessage> {

	DopechainMessage Handle(Net::OWNER_MESSAGE<DopechainMessage> _msg) override {
		return MessageFactory::Create(DopechainTypeMessage::SERVER_PING, DopechainStatusMessage::SUCCESS);
	}
};

struct RegistrationPeer : Net::MessageHandler<DopechainMessage> {

	std::shared_ptr<DopechainContainerPeers> container;

	RegistrationPeer(std::shared_ptr<DopechainContainerPeers> _container) : container{_container} {}

	bool Validate(Net::OWNER_MESSAGE<DopechainMessage> _msg) override {
		
		json jsonMessage = _msg->Message().ToJson();

		if (jsonMessage.empty()) {
			return false;
		}

		const auto& inner_obj = jsonMessage.begin().value();
		if (!inner_obj.contains("ADDRESS") || !inner_obj.contains("PORT")) {
			return false;
		}

		if (!inner_obj["ADDRESS"].is_string() || !inner_obj["PORT"].is_number_integer()) {
			return false;
		}
		
		return true;
	}

	DopechainMessage Handle(Net::OWNER_MESSAGE<DopechainMessage> _msg) override {

		json jsonMessage = _msg->Message().ToJson();
		DopechainPeerInfo info = jsonMessage.template get<DopechainPeerInfo>();
		
		if (container->Find(info)) {
			return MessageFactory::Create(DopechainTypeMessage::REGISTRATION_PEER, DopechainStatusMessage::FAILURE);
		}

		container->AddPeer(info);

		return MessageFactory::Create(DopechainTypeMessage::REGISTRATION_PEER, DopechainStatusMessage::SUCCESS);
	}
};

struct PeersList : Net::MessageHandler<DopechainMessage> {

	std::shared_ptr<DopechainContainerPeers> container;

	PeersList(std::shared_ptr<DopechainContainerPeers> _container) : container{ _container } {}

	DopechainMessage Handle(Net::OWNER_MESSAGE<DopechainMessage> _msg) override {

		json peerListJson = (*container);

		return MessageFactory::Create(DopechainTypeMessage::REGISTRATION_PEER, DopechainStatusMessage::SUCCESS, peerListJson);
	}
};