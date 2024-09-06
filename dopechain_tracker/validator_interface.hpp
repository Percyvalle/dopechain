#pragma once

#include <Definitions.hpp>

namespace Valid {

	class IValidator {
	public:
		virtual bool Validate(const json& _data) = 0;
		virtual ~IValidator() = default;
	};

}