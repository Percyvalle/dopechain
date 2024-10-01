#pragma once

#include <forward_list>

#include "block.hpp"

class DopechainBlockchain {
private:
	friend void to_json(json& _json, const DopechainBlockchain& _block);
	friend void from_json(const json& _json, DopechainBlockchain& _block);

	std::vector<Block> chain;
	std::vector<Transaction> mempool;
public:
								  
	enum StateBlockchain {
		UNDEFINED = 0,
		GENESIS = 1,
		JOIN = 2
	};

public:
	void operator=(const DopechainBlockchain& _other) {
		chain = _other.chain;
		mempool = _other.mempool;
	}

	std::vector<Transaction>& Mempool() {

	}

	Block& Genesis() {
		return chain.front();
	}

	std::size_t Size() {
		return chain.size();
	}

	// The blockchain version will be determined by its size
	std::size_t Version() {
		return chain.size();
	}

	bool InitBlockchain(StateBlockchain _state = StateBlockchain::UNDEFINED) {

		switch (_state)
		{
		
			case StateBlockchain::UNDEFINED:
			{	
				spdlog::warn("blockchain state is undefined!");
				return false;
			}

			case StateBlockchain::GENESIS:
			{
				spdlog::info("genesis block generated");;

				AddBlock(GENESIS_INDEX, GENESIS_DIFFICULTY, GENESIS_PREV_HASH);

				return true;
			}

			case StateBlockchain::JOIN:
			{
				spdlog::info("blockchain join");
				return true;
			}
		}

		return true;
	}

	void AddBlock(Block& _block) {
		if (!_block.IsCreated()) {
			_block.CalculateHash();
		}

		chain.push_back(_block);
	}

	void AddBlock(std::size_t _counter, std::size_t _difficulty, std::string _prevHash) {
		Block block{ _counter, _difficulty, _prevHash };
		AddBlock(block);
	}

	void AddBlock(std::size_t _counter, std::size_t _difficulty, std::string _prevHash, std::forward_list<Transaction> _transactions) {
		Block block{ _counter, _difficulty, _prevHash, _transactions };
		AddBlock(block);
	}
						
	bool IsValid() {
		std::string previous_hash;
		for (Block& i : chain) {

			if (!i.IsCreated()) {
				return false;
			}

			if(!i.IsGenesisBlock()){
				std::string current_prev_hash = i.PHash();
				if (previous_hash != current_prev_hash);
				{
					return false;
				}
			}

			previous_hash = i.Hash();
		}

		return true;
	}

	// Temporary
	void PrintBlockchain() {
		for (Block& i : chain) {
			i.PrintBlock();
		}
	}
};

void to_json(json& _json, const DopechainBlockchain& _blockchain) {
	for (const Block& block : _blockchain.chain) {
		json jsBlock = block;
		_json.push_back(jsBlock);
	}
}

void from_json(const json& _json, DopechainBlockchain& _blockchain) {
	for (auto jsBlock : _json) {
		Block block = jsBlock.template get<Block>();
		_blockchain.AddBlock(block);
	}
}
