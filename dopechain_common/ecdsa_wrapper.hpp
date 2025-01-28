#pragma once

#include "openssl/ec.h"
#include "openssl/ecdsa.h"
#include "openssl/bn.h"
#include "openssl/obj_mac.h"
#include "openssl/sha.h"
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <string>
#include <memory>

class DopechainSigner {
public:
	~DopechainSigner() {
	}
};