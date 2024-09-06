#pragma once

#include <string>

#include <utils/Utils.hpp>

#define MAX_DIGITS_UINT16 5;

inline UUIDv4::UUID GenerateUuidPeer(const std::string& _address, const std::uint16_t& _port) {
	std::string format = "%s:%d";
	std::size_t str_address_size = _address.size();
	std::size_t buffer_size = str_address_size + format.size() + MAX_DIGITS_UINT16;

	char* buffer = new char[buffer_size];

	sprintf_s(buffer, buffer_size, format.c_str(), _address.c_str(), _port);

	UUIDv4::UUID retUuid = UUIDv4::UUID::fromStrFactory(buffer);
	delete[] buffer;
	
	return retUuid;
}

struct DopechainPeerInfo {
	UUIDv4::UUID uuid;
	std::string address;
	std::uint16_t port;

	DopechainPeerInfo() = default;
	DopechainPeerInfo(const std::string& _address, const std::uint16_t& _port) : address(_address), port(_port)
	{
		uuid = GenerateUuidPeer(_address, _port);
	}

	bool operator==(const DopechainPeerInfo& _other) const {
		return address.compare(_other.address) == 0 && port == _other.port;
	}
};


struct DopechainPeerInfoHash {
	std::size_t operator()(const DopechainPeerInfo& _arg) const {
		std::size_t hash_address = std::hash<std::string>()(_arg.address);
		std::size_t hash_port = std::hash<std::uint16_t>()(_arg.port);

		return hash_address ^ (hash_port << 1);
	}
};

inline void to_json(json& _json, const DopechainPeerInfo& _info) {

	_json = json{ { _info.uuid.str(), {
					{"ADDRESS", _info.address},
					{"PORT", _info.port} } } };
}

inline void from_json(const json& _json, DopechainPeerInfo& _info) {
	if (_json.empty()) {
		spdlog::error("Invalid JSON format for DopechainPeerInfo in file {}:{}", __FILE__, __LINE__);
		return;
	}

	const auto& uuid_string = _json.begin().key();
	_info.uuid.fromStr(uuid_string.c_str());

	const auto& inner_obj = _json.begin().value();
	_info.port = inner_obj.at("PORT").get<std::uint16_t>();
	_info.address = inner_obj.at("ADDRESS").get<std::string>();
}
