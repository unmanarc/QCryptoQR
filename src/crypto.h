#ifndef CRYPTO_H
#define CRYPTO_H


#include <memory>
#include <string>
#include "mem.h"

namespace Mantids { namespace Helpers {


class Crypto
{
public:
    Crypto();


    static std::string AES256EncryptB64(const unsigned char * input, uint32_t inputLen, const char * key, uint32_t keyLen, bool * ok);
    static std::string AES256EncryptB64(const std::string & input, const char *key, uint32_t keyLen, bool * ok);
    static std::string AES256EncryptB64(const std::string & input, const std::string & key, bool * ok);

    static std::shared_ptr<Mem::xBinContainer> AES256DecryptB64ToBin(const std::string & input, const char *key, uint32_t keyLen, bool * ok);
    static std::string AES256DecryptB64(const std::string & input, const char *key, uint32_t keyLen, bool * ok);
    static std::string AES256DecryptB64(const std::string & input, const std::string & key, bool * ok);

    static std::string calcSHA256(const std::string & password);
    static std::string calcSHA512(const std::string & password);

    // SALT must be 4 bytes.
    static std::string calcSSHA256(const std::string & password, const unsigned char *ssalt);
    static std::string calcSSHA512(const std::string & password, const unsigned char *ssalt);
};

}}

#endif // CRYPTO_H
