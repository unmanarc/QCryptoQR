#include "random.h"

#include <random>

using namespace std;
using namespace Mantids::Helpers;

Random::Random()
{
}

string Random::createRandomString(string::basic_string::size_type length)
{
    // TODO: check randomness for crypto!
    char baseChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string randomStr;
    std::mt19937 rg{std::random_device{}()};
    std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(baseChars)-2);
    randomStr.reserve(length);
    while(length--) randomStr += baseChars[pick(rg)];
    return randomStr;
}

string Random::createRandomHexString(string::basic_string::size_type bytes)
{
    bytes = bytes*2;

    char baseChars[] = "ABCDEF0123456789";
    std::string randomStr;
    std::mt19937 rg{std::random_device{}()};
    std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(baseChars)-2);
    randomStr.reserve(bytes);
    while(bytes--) randomStr += baseChars[pick(rg)];
    return randomStr;
}

void Random::createRandomSalt32(unsigned char *salt)
{
    std::mt19937 rg{std::random_device{}()};
    std::uniform_int_distribution<uint32_t> pick;
    *((uint32_t *)salt) = pick(rg);
}

void Random::createRandomSalt128(unsigned char *salt)
{
    std::mt19937 rg{std::random_device{}()};
    std::uniform_int_distribution<uint64_t> pick;
    *((uint64_t *)salt) = pick(rg);
    *((uint64_t *)(salt+8)) = pick(rg);
}
