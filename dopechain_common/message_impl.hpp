#pragma once

#include <Definitions.hpp>
#include <network/MessageInterface.hpp>

#include "message_mapping.hpp"

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

	std::string StringType() {
		DopechainTypeMessage typeMessage = Type();
		if (messageTypeMapping.find(typeMessage) == messageTypeMapping.end()) {
			return "";
		}

		return messageTypeMapping.at(typeMessage);
	}

	std::string StringStatus() {
		DopechainStatusMessage statusMessage = Status();
		if (messageStatusMapping.find(statusMessage) == messageStatusMapping.end()) {
			return "";
		}

		return messageStatusMapping.at(statusMessage);
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