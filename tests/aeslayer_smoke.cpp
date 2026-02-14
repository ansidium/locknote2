#include "aeslayer.h"
#include "cryptopp/osrng.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	bool TryDecrypt(const std::vector<CryptoPP::byte>& cipher, const std::string& password, std::string& outPlaintext)
	{
		outPlaintext.clear();
		std::vector<CryptoPP::byte> output(cipher.size(), 0);
		const CryptoPP::ConstByteArrayParameter input(cipher.data(), cipher.size());
		const CryptoPP::DecodingResult result = CryptoPP::AESLayer::Decrypt(password, output.data(), input);
		if (!result.isValidCoding)
		{
			return false;
		}

		outPlaintext.assign(reinterpret_cast<const char*>(output.data()), result.messageLength);
		return true;
	}

	bool RoundTrip(const std::string& plaintext, const std::string& password, const CryptoPP::AESLayer::KdfMode mode)
	{
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::AESLayer::EncryptionOptions options;
		options.m_kdfMode = mode;

		std::vector<CryptoPP::byte> cipher(CryptoPP::AESLayer::MaxCiphertextLen(static_cast<unsigned int>(plaintext.size())), 0);
		const unsigned int cipherLen = CryptoPP::AESLayer::Encrypt(rng, password, cipher.data(), plaintext, options);
		cipher.resize(cipherLen);

		std::string decrypted;
		if (!TryDecrypt(cipher, password, decrypted))
		{
			return false;
		}
		return decrypted == plaintext;
	}

	bool TamperIsRejected(const std::string& plaintext, const std::string& password, const CryptoPP::AESLayer::KdfMode mode)
	{
		CryptoPP::AutoSeededRandomPool rng;
		CryptoPP::AESLayer::EncryptionOptions options;
		options.m_kdfMode = mode;

		std::vector<CryptoPP::byte> cipher(CryptoPP::AESLayer::MaxCiphertextLen(static_cast<unsigned int>(plaintext.size())), 0);
		const unsigned int cipherLen = CryptoPP::AESLayer::Encrypt(rng, password, cipher.data(), plaintext, options);
		cipher.resize(cipherLen);
		if (cipher.empty())
		{
			return false;
		}

		cipher[cipher.size() / 2] ^= static_cast<CryptoPP::byte>(0x5A);
		std::string decrypted;
		return !TryDecrypt(cipher, password, decrypted);
	}

	void Expect(const bool condition, const char* testName, int& failures)
	{
		if (condition)
		{
			std::cout << "[PASS] " << testName << '\n';
			return;
		}

		std::cout << "[FAIL] " << testName << '\n';
		++failures;
	}
}

int main()
{
	const std::string password = "correct horse battery staple";

	int failures = 0;
	Expect(RoundTrip("", password, CryptoPP::AESLayer::KdfMode::Scrypt), "roundtrip empty plaintext with scrypt", failures);
	Expect(RoundTrip("LockNote2 smoke payload", password, CryptoPP::AESLayer::KdfMode::Scrypt), "roundtrip non-empty plaintext with scrypt", failures);
	Expect(RoundTrip("LockNote2 smoke payload", password, CryptoPP::AESLayer::KdfMode::Pbkdf2Sha256), "roundtrip non-empty plaintext with PBKDF2-SHA256", failures);
	Expect(TamperIsRejected("tamper-detection", password, CryptoPP::AESLayer::KdfMode::Scrypt), "tampered ciphertext rejected", failures);

	if (failures != 0)
	{
		std::cout << "Smoke tests failed: " << failures << '\n';
		return 1;
	}

	std::cout << "All AES smoke tests passed." << '\n';
	return 0;
}
