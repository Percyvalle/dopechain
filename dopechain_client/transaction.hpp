#pragma once

#include <string>

class Transaction {

	friend void to_json(json& _json, const Transaction& _transaction);
	friend void from_json(const json& _json, Transaction& _transaction);

private:
	std::string hash;
	std::string amount;
	std::string sender;
	std::string recipient;
};

void to_json(json& _json, const Transaction& _transaction) {
	_json = json{ {"HASH", _transaction.hash},
				  {"AMOUNT", _transaction.amount},
				  {"SENDER", _transaction.sender},
				  {"RECIPIENT", _transaction.recipient}};
}

void from_json(const json& _json, Transaction& _transaction) {
	_json.at("HASH").get_to(_transaction.hash);
	_json.at("AMOUNT").get_to(_transaction.amount);
	_json.at("SENDER").get_to(_transaction.sender);
	_json.at("RECIPIENT").get_to(_transaction.recipient);
}