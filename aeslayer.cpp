#include <array>
#include <vector>
#include <cstring>
#include <algorithm>

#include "cryptopp/sha.h"
#include "cryptopp/pwdbased.h"
#include "cryptopp/scrypt.h"
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include "cryptopp/hmac.h"
#include "cryptopp/misc.h"

#include "aeslayer.h"

NAMESPACE_BEGIN(CryptoPP)

namespace
{
	constexpr std::array<byte, AESLayer::FORMAT_HEADER_SIZE - 1> kFormatMagic{ 'L', 'N', '2', 0x02 };
	constexpr unsigned int kScryptBlockSize = 8;
	constexpr unsigned int kScryptParallelization = 5;
	constexpr unsigned int kIvDerivationCost = 2;

	bool IsKnownKdfMode(const byte modeValue)
	{
		return modeValue == static_cast<byte>(AESLayer::KdfMode::Scrypt) ||
			modeValue == static_cast<byte>(AESLayer::KdfMode::Pbkdf2Sha256);
	}

	AESLayer::KdfMode ToKdfMode(const byte modeValue)
	{
		return modeValue == static_cast<byte>(AESLayer::KdfMode::Pbkdf2Sha256)
			? AESLayer::KdfMode::Pbkdf2Sha256
			: AESLayer::KdfMode::Scrypt;
	}

	void DeriveKeyAndIv(
		const AESLayer::KdfMode mode,
		ConstByteArrayParameter const& passphrase,
		const byte* salt,
		const byte* ivSeed,
		SecByteBlock& key,
		SecByteBlock& iv)
	{
		if (mode == AESLayer::KdfMode::Pbkdf2Sha256)
		{
			PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
			const byte purposeUnused = 0;
			pbkdf.DeriveKey(
				key.begin(),
				key.size(),
				purposeUnused,
				passphrase.begin(),
				passphrase.size(),
				salt,
				AESLayer::SALT_SIZE,
				AESLayer::KEY_ITERATIONS,
				0.0);

			pbkdf.DeriveKey(
				iv.begin(),
				iv.size(),
				purposeUnused,
				passphrase.begin(),
				passphrase.size(),
				ivSeed,
				AESLayer::IV_SEED_SIZE,
				AESLayer::KEY_ITERATIONS,
				0.0);
			return;
		}

		Scrypt scrypt;
		scrypt.DeriveKey(
			key.begin(),
			key.size(),
			passphrase.begin(),
			passphrase.size(),
			salt,
			AESLayer::SALT_SIZE,
			AESLayer::DERIVATION_COST,
			kScryptBlockSize,
			kScryptParallelization);

		scrypt.DeriveKey(
			iv.begin(),
			iv.size(),
			passphrase.begin(),
			passphrase.size(),
			ivSeed,
			AESLayer::IV_SEED_SIZE,
			kIvDerivationCost,
			kScryptBlockSize,
			kScryptParallelization);
	}

	bool ValidatePkcs7Padding(const byte* buffer, const size_t bufferSize, size_t& plainTextLength)
	{
		if (buffer == nullptr || bufferSize == 0)
		{
			return false;
		}

		const byte paddingByte = buffer[bufferSize - 1];
		if (paddingByte == 0 || paddingByte > AES::BLOCKSIZE || paddingByte > bufferSize)
		{
			return false;
		}

		for (size_t i = 0; i < paddingByte; ++i)
		{
			if (buffer[bufferSize - 1 - i] != paddingByte)
			{
				return false;
			}
		}

		plainTextLength = bufferSize - paddingByte;
		return true;
	}

	bool VerifyAndDecrypt(
		const AESLayer::KdfMode mode,
		ConstByteArrayParameter const& passphrase,
		const byte* authenticatedBegin,
		const size_t authenticatedSize,
		const byte* payload,
		const size_t payloadSize,
		const byte* salt,
		const byte* ivSeed,
		const byte* digest,
		byte* output,
		size_t& plainTextLength)
	{
		if (payloadSize == 0 || (payloadSize % AES::BLOCKSIZE) != 0)
		{
			return false;
		}

		SecByteBlock key(SHA256::DIGESTSIZE);
		SecByteBlock iv(AESLayer::IV_SIZE);
		DeriveKeyAndIv(mode, passphrase, salt, ivSeed, key, iv);

		std::array<byte, HMAC<SHA256>::DIGESTSIZE> checkDigest{};
		HMAC<SHA256>(key.begin(), key.size()).CalculateDigest(
			checkDigest.data(),
			authenticatedBegin,
			authenticatedSize);

		if (!VerifyBufsEqual(checkDigest.data(), digest, checkDigest.size()))
		{
			return false;
		}

		CBC_Mode<AES>::Decryption decryptor(key.begin(), key.size(), iv.begin());
		decryptor.ProcessData(output, payload, payloadSize);

		return ValidatePkcs7Padding(output, payloadSize, plainTextLength);
	}
}

unsigned int AESLayer::Encrypt(
	RandomNumberGenerator& rng,
	ConstByteArrayParameter const& passphrase,
	byte* output,
	const std::string& plaintext,
	const EncryptionOptions& options)
{
	const unsigned int paddingLength = AES::BLOCKSIZE - (plaintext.size() % AES::BLOCKSIZE);
	const unsigned int paddedSize = static_cast<unsigned int>(plaintext.size()) + paddingLength;

	std::vector<byte> paddedPlainText(paddedSize, 0);
	if (!plaintext.empty())
	{
		std::memcpy(paddedPlainText.data(), plaintext.data(), plaintext.size());
	}
	std::fill_n(paddedPlainText.data() + plaintext.size(), paddingLength, static_cast<byte>(paddingLength));

	byte* authenticatedBegin = nullptr;
	size_t authenticatedSize = 0;
	byte* salt = nullptr;
	byte* payload = nullptr;
	byte* ivSeed = nullptr;
	byte* digest = nullptr;

	// Keep default scrypt output in legacy format for backward compatibility.
	if (options.m_kdfMode == KdfMode::Scrypt)
	{
		salt = output;
		payload = salt + AESLayer::SALT_SIZE;
		ivSeed = payload + paddedSize;
		digest = ivSeed + AESLayer::IV_SEED_SIZE;
		authenticatedBegin = payload;
		authenticatedSize = static_cast<size_t>(digest - payload);
	}
	else
	{
		// New payload format:
		// [magic "LN2\x02"][kdf_mode][salt][ciphertext][iv_seed][digest]
		byte* magic = output;
		std::copy(kFormatMagic.begin(), kFormatMagic.end(), magic);
		byte* modeByte = magic + kFormatMagic.size();
		*modeByte = static_cast<byte>(options.m_kdfMode);
		salt = modeByte + 1;
		payload = salt + AESLayer::SALT_SIZE;
		ivSeed = payload + paddedSize;
		digest = ivSeed + AESLayer::IV_SEED_SIZE;
		authenticatedBegin = output;
		authenticatedSize = static_cast<size_t>(digest - output);
	}

	rng.GenerateBlock(salt, AESLayer::SALT_SIZE);
	rng.GenerateBlock(ivSeed, AESLayer::IV_SEED_SIZE);

	SecByteBlock key(SHA256::DIGESTSIZE);
	SecByteBlock iv(AESLayer::IV_SIZE);
	DeriveKeyAndIv(options.m_kdfMode, passphrase, salt, ivSeed, key, iv);

	CBC_Mode<AES>::Encryption encryptor(key.begin(), key.size(), iv.begin());
	encryptor.ProcessData(payload, paddedPlainText.data(), paddedSize);

	HMAC<SHA256>(key.begin(), key.size()).CalculateDigest(
		digest,
		authenticatedBegin,
		authenticatedSize);

	SecureWipeBuffer(paddedPlainText.data(), paddedPlainText.size());

	return static_cast<unsigned int>((digest + HMAC<SHA256>::DIGESTSIZE) - output);
}

DecodingResult AESLayer::Decrypt(ConstByteArrayParameter const& passphrase, byte* output, ConstByteArrayParameter const& input)
{
	if (input.size() < LEGACY_MINIMUM_CIPHERTEXT_LENGTH)
	{
		return DecodingResult();
	}

	const byte* begin = input.begin();
	const byte* end = input.end();

	// Preferred modern format with embedded KDF metadata.
	if (input.size() >= MINIMUM_CIPHERTEXT_LENGTH && std::equal(kFormatMagic.begin(), kFormatMagic.end(), begin))
	{
		const byte modeValue = begin[kFormatMagic.size()];
		if (IsKnownKdfMode(modeValue))
		{
			const byte* salt = begin + FORMAT_HEADER_SIZE;
			const byte* payload = salt + AESLayer::SALT_SIZE;
			const byte* digest = end - HMAC<SHA256>::DIGESTSIZE;
			const byte* ivSeed = digest - AESLayer::IV_SEED_SIZE;
			if (ivSeed > payload)
			{
				size_t plainTextLength = 0;
				if (VerifyAndDecrypt(
					ToKdfMode(modeValue),
					passphrase,
					begin,
					static_cast<size_t>(digest - begin),
					payload,
					static_cast<size_t>(ivSeed - payload),
					salt,
					ivSeed,
					digest,
					output,
					plainTextLength))
				{
					return DecodingResult(plainTextLength);
				}
			}
		}
	}

	// Legacy format fallback (without format header) for backward compatibility.
	const byte* salt = begin;
	const byte* payload = salt + AESLayer::SALT_SIZE;
	const byte* digest = end - HMAC<SHA256>::DIGESTSIZE;
	const byte* ivSeed = digest - AESLayer::IV_SEED_SIZE;
	if (ivSeed <= payload)
	{
		return DecodingResult();
	}

	size_t plainTextLength = 0;
	if (VerifyAndDecrypt(
		KdfMode::Scrypt,
		passphrase,
		payload,
		static_cast<size_t>(digest - payload),
		payload,
		static_cast<size_t>(ivSeed - payload),
		salt,
		ivSeed,
		digest,
		output,
		plainTextLength))
	{
		return DecodingResult(plainTextLength);
	}

	if (VerifyAndDecrypt(
		KdfMode::Pbkdf2Sha256,
		passphrase,
		payload,
		static_cast<size_t>(digest - payload),
		payload,
		static_cast<size_t>(ivSeed - payload),
		salt,
		ivSeed,
		digest,
		output,
		plainTextLength))
	{
		return DecodingResult(plainTextLength);
	}

	return DecodingResult();
}

NAMESPACE_END
