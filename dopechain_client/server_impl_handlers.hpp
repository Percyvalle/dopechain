#pragma once

#include <network/Handler.hpp>

#include "blockchain.hpp"

class Ping : public Net::MessageHandler<DopechainMessage> {
public:
	virtual DopechainMessage Handle(Net::OWNER_MESSAGE<DopechainMessage> _msg) {
		return MessageFactory::Create(DopechainTypeMessage::SERVER_PING, DopechainStatusMessage::SUCCESS);
	}
};

class BlockchainSync : public Net::MessageHandler<DopechainMessage> {
private:
	std::shared_ptr<DopechainBlockchain> blockchain;

public:
	BlockchainSync(std::shared_ptr<DopechainBlockchain> _blockchain) : blockchain{ _blockchain } {}

	virtual DopechainMessage Handle(Net::OWNER_MESSAGE<DopechainMessage> _msg) {
		
		json jsonBlockchain = *blockchain;

		spdlog::info(jsonBlockchain.dump());

		return MessageFactory::Create(DopechainTypeMessage::BLOCKCHAIN_SYNC, DopechainStatusMessage::SUCCESS, jsonBlockchain);
	}
};

class BlockchainVersion : public Net::MessageHandler<DopechainMessage> {
private:
	std::shared_ptr<DopechainBlockchain> blockchain;

public:
	BlockchainVersion(std::shared_ptr<DopechainBlockchain> _blockchain) : blockchain{ _blockchain } {}

	virtual DopechainMessage Handle(Net::OWNER_MESSAGE<DopechainMessage> _msg) {

		if (blockchain) {
			return MessageFactory::Create(DopechainTypeMessage::BLOCKCHAIN_VERSION, DopechainStatusMessage::SUCCESS, { {"VERSION", blockchain->Version()} });
		}

		return MessageFactory::Create(DopechainTypeMessage::BLOCKCHAIN_VERSION, DopechainStatusMessage::FAILURE);
	}
};