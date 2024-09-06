#pragma once

#include <vector>
#include <memory>

#include "peer_connection.hpp"

#define POINTER_PEER std::shared_ptr<DopechainPeer>

class DopechainPeerManager {
private:
	std::vector<POINTER_PEER> peers;

public:

	std::size_t Size() {
		return peers.size();
	}

	bool Empty() {
		return peers.empty();
	}

	bool ConnectToPeer(DopechainPeerInfo& _peerInfo) {
		POINTER_PEER connection = std::make_shared<DopechainPeer>(_peerInfo);

		connection->Connect(_peerInfo.address, _peerInfo.port);

		if (connection->IsConnected()) {
			peers.push_back(connection);
			return true;
		}

		return false;
	}

	bool ConnectToPeer(const DopechainContainerPeers& _container) {

		for (DopechainPeerInfo& peerInfo : _container.List()) {
			if (!ConnectToPeer(peerInfo)) {
				spdlog::warn("Failed connection to {}:{}", peerInfo.address, peerInfo.port);
				return false;
			}
		}

		return true;
	}

	bool ConnectToPeer(std::shared_ptr<DopechainContainerPeers> _container) {

		for (DopechainPeerInfo& peerInfo : _container->List()) {
			if (!ConnectToPeer(peerInfo)) {
				spdlog::warn("Failed connection to {}:{}", peerInfo.address, peerInfo.port);
				return false;
			}
		}

		return true;
	}

	void Disconnect(POINTER_PEER _peer) {
		peers.erase(std::remove_if(peers.begin(), peers.end(), [_peer](POINTER_PEER _othr) { return _peer == _othr; }), peers.end());
	}
};