#ifndef _AESLAYER_H_
#define _AESLAYER_H_

#include "cryptopp/algparam.h"
#include "cryptopp/aes.h"
#include "cryptopp/hmac.h"
#include "cryptopp/sha.h"

#include <string>

NAMESPACE_BEGIN(CryptoPP)

// AES Encryption Layer with confidentiality and authentication
// ==========================================================================
// This class uses CryptoPP's AES implementation in CBC mode, with PKCS#7
// padding and HMAC-SHA256 (encrypt-then-mac). The key/password is derived
// from the passphrase using a runtime-selectable KDF.
//
// The class is losely based on Denis Bider's AESPHM, with notable changes
// being standard PKCS#7 padding at the end of the plaintext, more resilient
// password derivation, use of CBC instead of CFB, and a modern hashing algo
// for HMAC generation.

class AESLayer
{
public:
	enum class KdfMode : byte
	{
		Scrypt = 1,
		Pbkdf2Sha256 = 2
	};

	struct EncryptionOptions
	{
		KdfMode m_kdfMode{ KdfMode::Scrypt };
	};

	static constexpr unsigned int MAX_PADDING_BYTES = AES::BLOCKSIZE;
	static constexpr unsigned int SALT_SIZE = 16;
	static constexpr unsigned int IV_SIZE = AES::BLOCKSIZE;
	static constexpr unsigned int IV_SEED_SIZE = IV_SIZE;

#ifdef _DEBUG
	static constexpr unsigned int DERIVATION_COST = 0x2000;
#else
	static constexpr unsigned int DERIVATION_COST = 0x4000;
#endif
	static constexpr unsigned int KEY_ITERATIONS = 0x10000;

	// "LN2\x02" + one byte for KDF mode.
	static constexpr unsigned int FORMAT_HEADER_SIZE = 5;
	static constexpr unsigned int LEGACY_MINIMUM_CIPHERTEXT_LENGTH = MAX_PADDING_BYTES + IV_SIZE + IV_SEED_SIZE + HMAC<SHA256>::DIGESTSIZE;
	static constexpr unsigned int MINIMUM_CIPHERTEXT_LENGTH = FORMAT_HEADER_SIZE + LEGACY_MINIMUM_CIPHERTEXT_LENGTH;

	// provides an upper limit, but not an exact limit, to the length of the ciphertext
	// that will be produced when encrypting plaintext of a specified size.
	static unsigned int MaxCiphertextLen(unsigned int plaintextLen) { return plaintextLen + MINIMUM_CIPHERTEXT_LENGTH; }

	// encryption:
	// use PKCS#7 padding to align to block size for AES CBC mode
	// derive a key from a salted passphrase using runtime-selected KDF
	// encrypt-then-mac using HMAC-SHA256
	// MAC is generated over the authenticated payload format
	// before: use MaxCiphertextLen() to allocate an output buffer of appropriate size
	static unsigned int Encrypt(RandomNumberGenerator& rng, ConstByteArrayParameter const& passphrase, byte* output, const std::string& plaintext, const EncryptionOptions& options = {});

	// decryption:
	// parse payload format, derive keys using selected or fallback KDF
	// check MAC tag by generating a HMAC-SHA256 over ciphertext and IV
	// then decrypt and remove padding
	// before: allocate an output buffer that is as large as the input
	static DecodingResult Decrypt(ConstByteArrayParameter const& passphrase, byte* output, ConstByteArrayParameter const& input);

};

NAMESPACE_END

#endif // _AESLAYER_H_
