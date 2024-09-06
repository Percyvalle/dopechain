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

	Block& Genesis() {
		return (*chain.begin());
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
				spdlog::info("genesis block generated");

				std::string prevHash(64, '0');

				Block genesisBlock{ GENESIS_INDEX, GENESIS_DIFFICULTY, prevHash };
				genesisBlock.CalculateHash();
				
				if (!genesisBlock.IsCreated()) {
					return false;
				}

				chain.push_back(genesisBlock);

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

	bool AddBlock(Block& _block) {
		if (_block.IsGenesisBlock()) {
			return false;
		}

		if (_block.IsCreated()) {
			chain.push_back(_block);
		}

		return true;
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