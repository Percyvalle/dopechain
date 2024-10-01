#pragma once

#include <vector>
#include <memory>
#include <queue>

#include "peer_connection.hpp"

class DopechainPeerManager {
private:
	std::shared_ptr<DopechainContainerPeers> containerPeers;
	std::vector<std::shared_ptr<DopechainPeer>> peersConnections;

public:
	DopechainPeerManager(std::shared_ptr<DopechainContainerPeers> _container) : containerPeers{_container} {
		for (DopechainPeerInfo& info : containerPeers->List()) {
			std::shared_ptr<DopechainPeer> peer = std::make_shared<DopechainPeer>(info);

			peer->Connect();

			if (!peer->IsConnected()) {
				spdlog::warn("Connection to {}:{} failed", peer->Address(), peer->Port());
			}
			else {
				peersConnections.push_back(peer);
			}
		}
	}

	bool TransactionNotification(std::shared_ptr<DopechainBlockchain> _blockchain) {

	}

	bool BlockchainSync(std::shared_ptr<DopechainBlockchain> _blockchain) {
		
		std::size_t maxVersion = NULL;
		std::shared_ptr<DopechainPeer> maxVersionPeer = nullptr;

		for (std::shared_ptr<DopechainPeer> peer : peersConnections) {
			Net::OWNER_MESSAGE<DopechainMessage> blockchainVersionResponse = peer->BlockchainVersion();

			if (blockchainVersionResponse != nullptr) {
				json message = blockchainVersionResponse->Message().ToJson();
				if (message.contains("VERSION")) {
					std::size_t version = message.at("VERSION").get<std::size_t>();
					if (maxVersion < version) {
						maxVersion = version;
						maxVersionPeer = peer;
					}
				}				
			}
		}

		if (maxVersionPeer == nullptr || maxVersion == NULL) {
			return false;
		}

		Net::OWNER_MESSAGE<DopechainMessage> blockchainSyncResponse = maxVersionPeer->BlockhcainSync();

		if (blockchainSyncResponse == nullptr) {
			return false;
		}

		*_blockchain = blockchainSyncResponse->Message().ToJson();;

		return true;
	}
};