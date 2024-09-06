#pragma once

#include <peers_info.hpp>

#include <unordered_map>

class DopechainContainerPeers {
private:
	friend inline void from_json(const json&, DopechainContainerPeers&);
	
	std::unordered_set<DopechainPeerInfo, DopechainPeerInfoHash> peers;

	mutable std::mutex containerMtx;
public:
	DopechainContainerPeers() = default;
	~DopechainContainerPeers() = default;

	DopechainContainerPeers(const DopechainContainerPeers& _other) {
		peers = _other.peers;
	}

	void operator=(const DopechainContainerPeers& _other) {
		std::lock_guard<std::mutex> lock(containerMtx);
		peers = _other.peers;
	}

	std::size_t Size() {
		std::lock_guard<std::mutex> lock(containerMtx);
		return peers.size();
	}

	void Erase(const DopechainPeerInfo& _info) {
		std::lock_guard<std::mutex> lock(containerMtx);
		peers.erase(_info);
	}

	bool Empty() {
		std::lock_guard<std::mutex> lock(containerMtx);
		return peers.empty();
	}

	bool AddPeer(DopechainPeerInfo& _peer) {
		std::lock_guard<std::mutex> lock(containerMtx);
		peers.insert(_peer);
		return true;
	}

	bool AddPeer(const std::string& _address, const std::uint16_t& _port) {
		std::lock_guard<std::mutex> lock(containerMtx);
		peers.emplace(_address, _port);
		return true;
	}

	bool Empty() const {
		std::lock_guard<std::mutex> lock(containerMtx);
		return peers.empty();
	}

	bool Find(DopechainPeerInfo& _peer){
		std::lock_guard<std::mutex> lock(containerMtx);
		return __Find(_peer);
	}

	bool Find(const std::string& _address, const std::uint16_t& _port) {
		std::lock_guard<std::mutex> lock(containerMtx);
		DopechainPeerInfo tempPeer(_address, _port);
		return __Find(tempPeer);
	}

	void Clear() {
		std::lock_guard<std::mutex> lock(containerMtx);
		peers.clear();
	}

	json Json() {
		std::lock_guard<std::mutex> lock(containerMtx);
		json returnJson = peers;
		return returnJson;
	}

	std::string String() {
		return Json().dump();
	}

	std::vector<DopechainPeerInfo> List() const {
		std::lock_guard<std::mutex> lock(containerMtx);
		std::vector<DopechainPeerInfo> list;

		for (DopechainPeerInfo i : peers) {
			list.push_back(i);
		}

		return list;
	}

private:
	bool __Find(DopechainPeerInfo& _peer) {
		if (peers.find(_peer) == peers.end()) {
			return false;
		}

		return true;
	}
};
																				

inline void to_json(json& _json, const DopechainContainerPeers& _container) {
	auto listPeer = _container.List();
	for (const DopechainPeerInfo& i : listPeer) {
		_json.emplace_back(i);
	}
}

inline void from_json(const json& _json, DopechainContainerPeers& _container) {
	for (auto i : _json) {
		DopechainPeerInfo info = i.template get<DopechainPeerInfo>();
		_container.AddPeer(info);
	}
}

