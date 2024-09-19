#pragma once

#include <merkle/merklecpp.h>
#include <fort/fort.hpp>

#include <utils/Utils.hpp>

#include "transaction.hpp"

#define GENESIS_INDEX NULL
#define GENESIS_DIFFICULTY 3
#define GENESIS_PREV_HASH std::string(64, '0')

using MerkleTree = merkle::Tree;

class Block {
private:

	friend void to_json(json& _json, const Block& _block);
	friend void from_json(const json& _json, Block& _block);

	struct HeaderBlock {
		std::string	nonce;
		std::size_t counter;
		std::string currHash;
		std::string prevHash;
		std::size_t difficulty;
		std::string timeCreate;
		MerkleTree merkleRoot;
	} headerBlock;

public:
	Block() = default;

	Block(std::size_t _counter, std::size_t _difficulty, std::string _prevHash, std::forward_list<Transaction> _transactions) {
		headerBlock.counter = _counter;
		headerBlock.prevHash = _prevHash;
		headerBlock.difficulty = _difficulty;
	}

	Block(std::size_t _counter, std::size_t _difficulty, std::string _prevHash) {
		headerBlock.counter = _counter;
		headerBlock.prevHash = _prevHash;
		headerBlock.difficulty = _difficulty;
	}

	std::string Hash() {
		return headerBlock.currHash;
	}

	std::string PHash() {
		return headerBlock.prevHash;
	}

	bool IsGenesisBlock() {
		if (headerBlock.counter == GENESIS_INDEX) {
			return true;
		}

		return false;
	}

	void CalculateHash() {
		std::string counter_str = std::to_string(headerBlock.counter);
		std::string difficulty_str = std::to_string(headerBlock.difficulty);

		std::string merkleRoot_str;
		if (!headerBlock.merkleRoot.empty()) {
			merkleRoot_str = headerBlock.merkleRoot.root().to_string();
		}
		else {
			merkleRoot_str = "empty";
		}

		std::string header_str = counter_str + difficulty_str + merkleRoot_str + headerBlock.prevHash + headerBlock.timeCreate;

		for (int i = 0; i < INT_MAX; i++) {
			std::string nonce = std::to_string(i);
			std::string hash = Utils::Sha256(header_str + nonce);
			spdlog::debug("mining.... {}", hash);
			if (hash.substr(0, headerBlock.difficulty) == std::string(headerBlock.difficulty, '0')) {
				headerBlock.nonce = nonce;
				headerBlock.currHash = hash;
				headerBlock.timeCreate = Utils::GetTimestamp();
				spdlog::debug("mining completed successfully, Nonce: {}", nonce);
				break;
			}
		}
	}

	bool IsCreated() {
		return !headerBlock.currHash.empty();
	}

	void PrintBlock() {
		if (!IsCreated()) {
			spdlog::warn("block is not created");
			return;
		}

		fort::table blocktable;
		blocktable << fort::header << "COUNTER: " << headerBlock.counter << fort::endr
								   << "TIME_CERATE" << headerBlock.timeCreate << fort::endr
								   << "HASH: " << headerBlock.currHash << fort::endr
								   << "PREV_HASH: " << headerBlock.prevHash << fort::endr
								   << "DIFFICULTY: " << headerBlock.difficulty << fort::endr
								   << "NONCE: " << headerBlock.nonce << fort::endr;
		std::cout << blocktable.to_string();
	}
};

void to_json(json& _json, const Block& _block) {
	_json = json{ {"NONCE", _block.headerBlock.nonce},
				  {"HASH", _block.headerBlock.currHash},
				  {"COUNTER", _block.headerBlock.counter},
				  {"PREV_HASH", _block.headerBlock.prevHash},
				  {"DIFFICULTY", _block.headerBlock.difficulty},
				  {"TIME_CREATE", _block.headerBlock.timeCreate}};
}

void from_json(const json& _json, Block& _block) {
	_json.at("NONCE").get_to(_block.headerBlock.nonce);
	_json.at("HASH").get_to(_block.headerBlock.currHash);
	_json.at("COUNTER").get_to(_block.headerBlock.counter);
	_json.at("PREV_HASH").get_to(_block.headerBlock.prevHash);
	_json.at("DIFFICULTY").get_to(_block.headerBlock.difficulty);
	_json.at("TIME_CREATE").get_to(_block.headerBlock.timeCreate);
}