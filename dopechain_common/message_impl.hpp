#pragma once

#include <Definitions.hpp>
#include <network/MessageInterface.hpp>

enum DopechainTypeMessage {
	PEERS_LIST = 200,
	SERVER_PING = 210,
	REGISTRATION_PEER = 220,
	BLOCKCHAIN_SYNC = 230
};

enum DopechainStatusMessage {
	CLIENT_STATUS = 100,
	UNDEFINED = 110,
	SUCCESS = 120,
	FAILURE	= 130
};


struct DopechainHeader : Net::IHeader<DopechainTypeMessage, DopechainStatusMessage> {};
struct DopechainBody : Net::IBody {};

struct DopechainMessage : Net::IMessage<DopechainHeader, DopechainBody> {
	
	void FromJson(json _data) {
		std::string buffer = _data.dump();
		std::size_t buffer_size = buffer.size();

		body.Resize(buffer_size);
		header.SetSize(buffer_size);

		std::memcpy(body.Data().data(), buffer.data(), buffer_size);
	}

	json ToJson() {
		return IsJson() ? json::parse(ToString()) : json();
	}

	bool IsJson() {
		return json::accept(ToString());
	}

	DopechainTypeMessage Type() {
		return Header().Type();
	}

	DopechainStatusMessage Status() {
		return Header().Status();
	}

	std::string ToString() {
		if (!body.Empty()) {
			std::string buffer;

			buffer.resize(body.Size());
			
			memcpy(buffer.data(), body.Data().data(), body.Size());

			return buffer;
		}

		return "";
	}

};

struct MessageFactory {

	static DopechainMessage Create(DopechainTypeMessage _type, DopechainStatusMessage _status, json _data = nullptr) {

		DopechainMessage message;
		message.Header().SetType(_type);
		message.Header().SetStatus(_status);

		if (_data != nullptr) {
			message.FromJson(_data);
		}

		return message;
	}

};