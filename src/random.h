#ifndef HLP_RANDOM_H
#define HLP_RANDOM_H

#include <string>
#include <random>

namespace Mantids { namespace Helpers {

class Random
{
public:
    Random();
    /**
     * @brief createRandomString Create a Random String (letters and numbers)
     * @param length output lenght
     * @return random string with letters (mayus/minus) and numbers
     */
    static std::string createRandomString(std::string::size_type length);
    /**
     * @brief createRandomHexString Create a Random String (hexadecimal characters only)
     * @param bytes will generate 2 HEX bytes per byte requested (eg. 2 -> ABCD)
     * @return random string in hex
     */
    static std::string createRandomHexString(std::string::size_type bytes);

    /**
     * @brief createRandomSalt32 Create a random number of 32bit into salt
     * @param salt output variable.
     */
    static void createRandomSalt32(unsigned char * salt);

    /**
     * @brief createRandomSalt128 Create a random number of 128bit into salt
     * @param salt output variable.
     */
    static void createRandomSalt128(unsigned char * salt);

    /**
     * @brief safe_random_shuffle Suffle a vector
     * @param first vector begin()
     * @param last vector end()
     * @param hash Hash function
     */
    template <class RandomAccessIterator> static void safe_random_shuffle (RandomAccessIterator first, RandomAccessIterator last, std::size_t hash)
    {
        std::minstd_rand0 gen(hash);
        typename std::iterator_traits<RandomAccessIterator>::difference_type i, n;
        n = (last-first);
        for (i=n-1; i>0; --i)
        {
            std::uniform_int_distribution<> dis(0, static_cast<int>(i));
            std::swap (first[i],first[dis(gen)]);
        }
    }
private:
};

}}

#endif // HLP_RANDOM_H
