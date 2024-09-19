#pragma once

#include <unordered_map>
#include <string>

enum class DopechainTypeMessage : int {
	PEERS_LIST = 200,
	SERVER_PING = 210,
	REGISTRATION_PEER = 220,
	BLOCKCHAIN_SYNC = 230,
	BLOCKCHAIN_VERSION = 240
};

enum class DopechainStatusMessage : int{
	CLIENT_STATUS = 100,
	UNDEFINED = 110,
	SUCCESS = 120,
	FAILURE = 130
};

static const std::unordered_map<DopechainTypeMessage, std::string> messageTypeMapping = {
	{ DopechainTypeMessage::PEERS_LIST, "PEERS_LIST" },
	{ DopechainTypeMessage::SERVER_PING, "SERVER_PING" },
	{ DopechainTypeMessage::BLOCKCHAIN_SYNC, "BLOCKCHAIN_SYNC" },
	{ DopechainTypeMessage::REGISTRATION_PEER, "REGISTRATION_PEER" },
	{ DopechainTypeMessage::BLOCKCHAIN_VERSION, "BLOCKCHAIN_VERSION" }
};

static const std::unordered_map<DopechainStatusMessage, std::string> messageStatusMapping = {
	{ DopechainStatusMessage::FAILURE, "FAILURE" },
	{ DopechainStatusMessage::SUCCESS, "SUCCESS" },
	{ DopechainStatusMessage::UNDEFINED, "UNDEFINED" },
	{ DopechainStatusMessage::CLIENT_STATUS, "CLIENT_STATUS" }
};