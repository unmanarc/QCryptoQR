#ifndef HLP_MEM_H
#define HLP_MEM_H

#include <string.h>
#include <stdint.h>
#include <limits>
#include <string>

namespace Mantids { namespace Helpers {

#define MAX_SIZE_T std::numeric_limits<std::size_t>::max()

#define CHECK_UINT_OVERFLOW_SUM(a,b) (a+b<a || a+b<b)
#define CHECK_UINT_OVERFLOW_REM(a,b) (b>a)

#define KB_MULT (1024)
#define MB_MULT (KB_MULT*1024)
#define GB_MULT (MB_MULT*1024)
#define TB_MULT (GB_MULT*1024)

//#define ZeroBArray(x) memset((x),0,sizeof((x)));
//#define ZeroBStruct(x) memset(&(x),0,sizeof(x));

#ifdef HAVE_EXPLICIT_BZERO
#define ZeroBArray(x) explicit_bzero((x),sizeof((x)));
#define ZeroBStruct(x) explicit_bzero(&(x),sizeof(x));
#define SecBACopy(dst,src) explicit_bzero(&dst, sizeof(dst)); strncpy(dst, src, sizeof(dst)-1);
#elif _WIN32
#define ZeroBArray(x) SecureZeroMemory((x),sizeof((x)));
#define ZeroBStruct(x) SecureZeroMemory(&(x),sizeof(x));
#define SecBACopy(dst,src) SecureZeroMemory(&dst, sizeof(dst)); strncpy(dst, src, sizeof(dst)-1);
#else
#define ZeroBArray(x) memset((x),0,sizeof((x)));
#define ZeroBStruct(x) memset(&(x),0,sizeof(x));
#define SecBACopy(dst,src) memset(&dst,0, sizeof(dst)); strncpy(dst, src, sizeof(dst)-1);
#endif

#define ZeroBArrayNS(x) memset((x),0,sizeof((x)));
#define ZeroBStructNS(x) memset(&(x),0,sizeof(x));

class Mem
{
public:
    Mem();

    struct xBinContainer
    {
        xBinContainer(const char * data , const uint64_t &len);
        xBinContainer(const uint64_t &len);
        ~xBinContainer();

        std::string toString()
        {
            std::string r((char *)data,len);
            return r;
        }

        void operator+=( const unsigned char & c );

        void * data;
        uint64_t len, cur;
    };

    static bool icharcmp(unsigned char c1,unsigned  char c2);
    static int memcmp64(const void *s1, const void *s2, uint64_t n);
    static int memicmp2(const void *s1, const void *s2, const uint64_t &n, const bool & caseSensitive);
    static void *memcpy64(void *dest, const void *src, uint64_t n);
    static void *memmove64(void *dest, const void *src, uint64_t n);


private:

    static unsigned char cmpMatrix[];

};

}}

#endif // HLP_MEM_H
