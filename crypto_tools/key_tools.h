#ifndef KEYTOOL_H
#define KEYTOOL_H
#include <string>
#include <array>
extern const uint32_t PRI_KEY_SIZE;
extern const uint32_t PUB_KEY_SIZE;

// wallet key tools
std::string CreatePrivateKey();
std::string GetPublicKeyByPrivateKey(const std::string private_key_org);
std::string GetSignByPrivateKey(const uint8_t* buf, size_t length, const std::string pri_key);

// encode decode tools
std::string CreateAesIVKey();
bool AesEncode(const std::string& key, const std::string& iv, const std::string& in, std::string& out);
bool AesDecode(const std::string& key, const std::string& iv, const std::string& in, std::string& out);

bool SignIsValidate(const uint8_t* buf, size_t length, const std::string& pub_key, const std::string& sign);
#endif // KEYTOOL_H
