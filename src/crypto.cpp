#include "crypto.h"
#include "encoders.h"
#include "random.h"

#include <cstring>
#include <openssl/evp.h>
#include <openssl/ossl_typ.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <sys/types.h>

using namespace Mantids::Helpers;


std::string Crypto::AES256EncryptB64(const unsigned char * input, uint32_t inputLen, const char * key, uint32_t keyLen, bool * ok)
{
    std::string out;
    if (ok)
        *ok = false;

    // Create the random salt (128bit) and derived Key (256bit)...
    uint8_t salt[128/8],derivedKey[256/8];

    Helpers::Random::createRandomSalt128(salt);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int err;

    if (!ctx)
        return out;

    // Derive the key...
    if (PKCS5_PBKDF2_HMAC( key , keyLen, salt, sizeof(salt), 100000, EVP_sha256(), sizeof(derivedKey), derivedKey  ) == 1)
    {
        // Initialize the encryption...
        if ((err = EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, derivedKey, salt)) == 1)
        {
            if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
            {
                int len;
                uint32_t cipherOutLength = 32;
                uint8_t* cipherOutText = new uint8_t[(inputLen * 2)+32];

                memcpy(cipherOutText,salt,16);
                if (EVP_EncryptUpdate(ctx, cipherOutText + cipherOutLength, &len, input, inputLen) == 1 && len>=0)
                {
                    cipherOutLength += len;
                    if (EVP_EncryptFinal_ex(ctx, cipherOutText + cipherOutLength, (int*)&len) == 1 && len>=0)
                    {
                        cipherOutLength += len;
                        std::uint8_t gcmTag[16];

                        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, gcmTag))
                        {
                            memcpy(cipherOutText+16,gcmTag,16);
                            out = Helpers::Encoders::toBase64( cipherOutText, cipherOutLength );
                            if (ok)
                                *ok = true;
                        }
                    }
                }
                delete[] cipherOutText;
            }
        }
    }
    EVP_CIPHER_CTX_free(ctx);
    return out;
}

std::string Crypto::AES256EncryptB64(const std::string &input, const char *key, uint32_t keyLen, bool *ok)
{
    return AES256EncryptB64((unsigned char *)input.c_str(),input.length(),key,keyLen,ok);
}

std::string Crypto::AES256EncryptB64(const std::string &input,  const std::string &key, bool *ok)
{
    return AES256EncryptB64((unsigned char *)input.c_str(),input.length(),key.c_str(),key.length(),ok);
}

std::shared_ptr<Mem::xBinContainer> Crypto::AES256DecryptB64ToBin(const std::string &input, const char *key, uint32_t keyLen, bool *ok)
{
    if (ok)
        *ok = false;

    auto r = std::make_shared<Mem::xBinContainer>( input.length() );

    if (!r->data)
        return r;

    auto dec = Helpers::Encoders::fromBase64ToBin(input);

    if (dec->data && dec->cur>=32)
    {
        uint8_t salt[128/8],derivedKey[256/8],gcmTag[16];
        unsigned char * encryptedData = ((unsigned char *)dec->data)+32;
        uint64_t encryptedDataLen;
        memcpy(salt,dec->data,16);
        memcpy(gcmTag,((unsigned char *)dec->data)+16,16);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        int err;

        if (!ctx)
            return r;

        // Derive the key...
        if (PKCS5_PBKDF2_HMAC( key , keyLen, salt, sizeof(salt), 100000, EVP_sha256(), sizeof(derivedKey), derivedKey  ) == 1)
        {
            // Initialize the encryption...
            if ((err = EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, derivedKey, salt)) == 1)
            {
                if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
                {
                    int len=-1;
                    if (EVP_DecryptUpdate(ctx, (unsigned char *) r->data, &len,  (unsigned char *) dec->data+32, dec->cur-32 ) == 1 && len>=0)
                    {
                        r->cur+=len;

                        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)gcmTag))
                        {
                            if ((err = EVP_DecryptFinal_ex(ctx, (unsigned char *) r->data + r->cur, &len)) == 1 && len>=0)
                            {
                                r->cur += len;

                                // OK...
                                if (ok)
                                    *ok = true;
                            }
                        }
                    }
                    else
                    {
                        int o=0;
                        o++;
                    }
                }
            }
        }
        EVP_CIPHER_CTX_free(ctx);
    }
    return r;
}


std::string Crypto::AES256DecryptB64(const std::string &input,  const char * key, uint32_t keyLen, bool *ok)
{
    return AES256DecryptB64ToBin(input,key,keyLen,ok)->toString();
}
std::string Crypto::AES256DecryptB64(const std::string &input,  const std::string & key, bool *ok)
{
    return AES256DecryptB64ToBin(input,key.c_str(),key.length(),ok)->toString();
}

std::string Crypto::calcSHA256(const std::string &password)
{
    std::string r;
    unsigned char buffer_sha2[SHA256_DIGEST_LENGTH+1];
    SHA256_CTX sha2;
    SHA256_Init (&sha2);
    SHA256_Update (&sha2, (const unsigned char *) password.c_str(), password.length());
    SHA256_Final ( buffer_sha2, &sha2);
    return Encoders::toHex(buffer_sha2,SHA256_DIGEST_LENGTH);
}

std::string Crypto::calcSHA512(const std::string &password)
{
    std::string r;
    unsigned char buffer_sha2[SHA512_DIGEST_LENGTH+1];
    SHA512_CTX sha2;
    SHA512_Init (&sha2);
    SHA512_Update (&sha2, (const unsigned char *) password.c_str(), password.length());
    SHA512_Final ( buffer_sha2, &sha2);
    return Encoders::toHex(buffer_sha2,SHA512_DIGEST_LENGTH);
}

std::string Crypto::calcSSHA256(const std::string &password, const unsigned char * ssalt)
{
    std::string r;
    unsigned char buffer_sha2[SHA256_DIGEST_LENGTH+1];
    SHA256_CTX sha2;
    SHA256_Init (&sha2);
    SHA256_Update (&sha2, (const unsigned char *) password.c_str(), password.length());
    SHA256_Update (&sha2, ssalt, 4);
    SHA256_Final ( buffer_sha2, &sha2);
    return Encoders::toHex(buffer_sha2,SHA256_DIGEST_LENGTH);
}

std::string Crypto::calcSSHA512(const std::string &password, const unsigned char * ssalt)
{
    std::string r;
    unsigned char buffer_sha2[SHA512_DIGEST_LENGTH+1];
    SHA512_CTX sha2;
    SHA512_Init (&sha2);
    SHA512_Update (&sha2, (const unsigned char *) password.c_str(), password.length());
    SHA512_Update (&sha2, ssalt, 4);
    SHA512_Final ( buffer_sha2, &sha2);
    return Encoders::toHex(buffer_sha2,SHA512_DIGEST_LENGTH);
}
