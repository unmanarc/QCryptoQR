#include "encoders.h"
#include <string.h>
#include <random>
#include <inttypes.h>

using namespace std;
using namespace Mantids::Helpers;

const std::string Encoders::b64Chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

Encoders::Encoders()
{
}

string Encoders::fromBase64Obf(const string &sB64Buf, const uint64_t & seed)
{
    unsigned char cont4[4], cont3[3];
    std::string decodedString;
    std::mt19937_64 gen( seed );
    std::uniform_int_distribution<char> dis;
    size_t count=sB64Buf.size(), x=0, y=0;
    int bufPos=0;

    while (     count-- &&
                ( sB64Buf[bufPos] != '=')  &&
                (isalnum(sB64Buf[bufPos]) || (sB64Buf[bufPos] == '/') || (sB64Buf[bufPos] == '+'))
           )
    {
        cont4[x++]=sB64Buf[bufPos]; bufPos++;
        if (x==4)
        {
            for (x=0; x <4; x++)
            {
                cont4[x]=(unsigned char)b64Chars.find(cont4[x]);
            }

            cont3[0]=(cont4[0] << 2) + ((cont4[1] & 0x30) >> 4);
            cont3[1]=((cont4[1] & 0xf) << 4) + ((cont4[2] & 0x3c) >> 2);
            cont3[2]=((cont4[2] & 0x3) << 6) + cont4[3];

            for (x=0; (x < 3); x++)
            {
                decodedString += cont3[x]^dis(gen);
            }
            x=0;
        }
    }

    if (x)
    {
        for (y=x; y <4; y++)
        {
            cont4[y]=0;
        }
        for (y=0; y <4; y++)
        {
            cont4[y]=(unsigned char)b64Chars.find(cont4[y]);
        }

        cont3[0]=(cont4[0] << 2) + ((cont4[1] & 0x30) >> 4);
        cont3[1]=((cont4[1] & 0xf) << 4) + ((cont4[2] & 0x3c) >> 2);
        cont3[2]=((cont4[2] & 0x3) << 6) + cont4[3];

        for (y=0; (y < x - 1); y++) decodedString += cont3[y]^dis(gen);
    }

    return decodedString;
}

string Encoders::toBase64Obf(const unsigned char *buf, uint64_t count,  const uint64_t & seed)
{
    std::string r;
    std::mt19937_64 gen( seed );
    std::uniform_int_distribution<char> dis;

    unsigned char * obfBuf = (unsigned char *)malloc(count);
    if (!obfBuf) return "";

    for ( size_t i=0; i<count; i++ )
        obfBuf[i] = buf[i]^dis(gen);

    r = toBase64(obfBuf,count);
    free(obfBuf);
    return r;
}

string Encoders::toBase64Obf(const string &buf, const uint64_t & seed)
{
    return toBase64Obf((unsigned char *)buf.c_str(),buf.size(),seed);
}

std::shared_ptr<Mem::xBinContainer> Encoders::fromBase64ToBin(const std::string &sB64Buf)
{
    auto r = std::make_shared<Mem::xBinContainer>( sB64Buf.length() );
    if (!r->data)
        return r;

    unsigned char cont4[4], cont3[3];
    uint64_t count=sB64Buf.size(), x=0, y=0;
    int bufPos=0;

    while (     count-- &&
                ( sB64Buf[bufPos] != '=')  &&
                (isalnum(sB64Buf[bufPos]) || (sB64Buf[bufPos] == '/') || (sB64Buf[bufPos] == '+'))
           )
    {
        cont4[x++]=sB64Buf[bufPos]; bufPos++;
        if (x==4)
        {
            for (x=0; x <4; x++)
            {
                cont4[x]=(unsigned char)b64Chars.find(cont4[x]);
            }

            cont3[0]=(cont4[0] << 2) + ((cont4[1] & 0x30) >> 4);
            cont3[1]=((cont4[1] & 0xf) << 4) + ((cont4[2] & 0x3c) >> 2);
            cont3[2]=((cont4[2] & 0x3) << 6) + cont4[3];

            for (x=0; (x < 3); x++)
            {
                *r += cont3[x];
            }
            x=0;
        }
    }

    if (x)
    {
        for (y=x; y <4; y++)
        {
            cont4[y]=0;
        }
        for (y=0; y <4; y++)
        {
            cont4[y]=(unsigned char)b64Chars.find(cont4[y]);
        }

        cont3[0]=(cont4[0] << 2) + ((cont4[1] & 0x30) >> 4);
        cont3[1]=((cont4[1] & 0xf) << 4) + ((cont4[2] & 0x3c) >> 2);
        cont3[2]=((cont4[2] & 0x3) << 6) + cont4[3];

        for (y=0; (y < x - 1); y++) *r += cont3[y];
    }

    return r;
}

string Encoders::fromBase64(const string &sB64Buf)
{
    unsigned char cont4[4], cont3[3];
    std::string decodedString;
    uint64_t count=sB64Buf.size(), x=0, y=0;
    int bufPos=0;

    while (     count-- &&
                ( sB64Buf[bufPos] != '=')  &&
                (isalnum(sB64Buf[bufPos]) || (sB64Buf[bufPos] == '/') || (sB64Buf[bufPos] == '+'))
           )
    {
        cont4[x++]=sB64Buf[bufPos]; bufPos++;
        if (x==4)
        {
            for (x=0; x <4; x++)
            {
                cont4[x]=(unsigned char)b64Chars.find(cont4[x]);
            }

            cont3[0]=(cont4[0] << 2) + ((cont4[1] & 0x30) >> 4);
            cont3[1]=((cont4[1] & 0xf) << 4) + ((cont4[2] & 0x3c) >> 2);
            cont3[2]=((cont4[2] & 0x3) << 6) + cont4[3];

            for (x=0; (x < 3); x++)
            {
                decodedString += cont3[x];
            }
            x=0;
        }
    }

    if (x)
    {
        for (y=x; y <4; y++)
        {
            cont4[y]=0;
        }
        for (y=0; y <4; y++)
        {
            cont4[y]=(unsigned char)b64Chars.find(cont4[y]);
        }

        cont3[0]=(cont4[0] << 2) + ((cont4[1] & 0x30) >> 4);
        cont3[1]=((cont4[1] & 0xf) << 4) + ((cont4[2] & 0x3c) >> 2);
        cont3[2]=((cont4[2] & 0x3) << 6) + cont4[3];

        for (y=0; (y < x - 1); y++) decodedString += cont3[y];
    }

    return decodedString;
}

string Encoders::toBase64(const string &buf)
{
    return toBase64((unsigned char *)buf.c_str(),buf.size());
}

string Encoders::toBase64(const unsigned char *buf, uint64_t count)
{
    unsigned char cont3[3],cont4[4];
    std::string encodedString;
    int x=0, y=0;

    while (count--)
    {
        cont3[x++]=*(buf++);
        if (x == 3)
        {
            cont4[0]=(cont3[0] & 0xfc) >> 2;
            cont4[1]=((cont3[0] & 0x03) << 4) + ((cont3[1] & 0xf0) >> 4);
            cont4[2]=((cont3[1] & 0x0f) << 2) + ((cont3[2] & 0xc0) >> 6);
            cont4[3]=cont3[2] & 0x3f;

            for(x=0; (x <4) ; x++)
            {
                encodedString += b64Chars[cont4[x]];
            }

            x=0;
        }
    }

    if (x)
    {
        for(y=x; y < 3; y++)
        {
            cont3[y]=0;
        }

        cont4[0]=(cont3[0] & 0xfc) >> 2;
        cont4[1]=((cont3[0] & 0x03) << 4) + ((cont3[1] & 0xf0) >> 4);
        cont4[2]=((cont3[1] & 0x0f) << 2) + ((cont3[2] & 0xc0) >> 6);
        cont4[3]=cont3[2] & 0x3f;

        for (y=0; (y < x + 1); y++)
        {
            encodedString += b64Chars[cont4[y]];
        }

        while((x++ < 3)!=0)
        {
            encodedString += '=';
        }
    }

    return encodedString;
}

string Encoders::toURL(const string &str, const eURLEncodingType & urlEncodingType)
{
    if (!str.size()) return "";

    size_t x=0;
    std::string out;
    out.resize(calcURLEncodingExpandedStringSize(str,urlEncodingType),' ');

    for (size_t i=0; i<str.size();i++)
    {
        if ( getIfMustBeURLEncoded(str.at(i),urlEncodingType) )
        {
            out[x++]='%';
            out[x++]=toHexPair(str.at(i), 1);
            out[x++]=toHexPair(str.at(i), 2);
        }
        else
        {
            out[x++] = str.at(i);
        }
    }
    return out;
}

string Encoders::fromURL(const string &urlEncodedStr)
{
    std::string r;
    if (!urlEncodedStr.size()) return "";

    for (size_t i=0; i<urlEncodedStr.size();i++)
    {
        if ( urlEncodedStr[i] == '%' && i+3<=urlEncodedStr.size() && isHexChar(urlEncodedStr[i+1]) && isHexChar(urlEncodedStr[i+2]) )
        {
            char v = hexToValue(urlEncodedStr[i+1])*0x10 + hexToValue(urlEncodedStr[i+2]);
            r+=v;
            i+=2;
        }
        else
        {
            r+=urlEncodedStr[i];
        }
    }
    return r;
}

string Encoders::toHex(const unsigned char *data, size_t len)
{
    std::string r;
    for (size_t x = 0; x<len; x++)
    {
        char buf[4];
        sprintf(buf, "%02" PRIX8, data[x]);
        r.append( buf );
    }
    return r;
}

void Encoders::fromHex(const string &hexValue, unsigned char *data, size_t maxlen)
{
    if ((hexValue.size()/2)<maxlen) maxlen=(hexValue.size()/2);
    for (size_t i=0;i<(maxlen*2);i+=2)
    {
        data[i/2] = hexToValue(hexValue.at(i))*0x10 + hexToValue(hexValue.at(i+1));
    }
}

char Encoders::toHexPair(char value, char part)
{
    if (part == 1) value = value/0x10;
    else if (part == 2) value = value&0xF;
    if (value >= 0x0 && value <=0x9) return '0'+value;
    if (value >= 0xA && value <=0xF) return 'A'+value-0xA;
    return '0';
}

bool Encoders::isHexChar(char v)
{
    return (v>='0' && v<='9') || (v>='A' && v<='F');
}

char Encoders::hexToValue(char v)
{
    if (v>='0' && v<='9') return v-'0';
    if (v>='A' && v<='F') return v-'A'+10;
    if (v>='a' && v<='f') return v-'a'+10;
    return 0;
}

bool Encoders::getIfMustBeURLEncoded(char c,const eURLEncodingType & urlEncodingType)
{
    if (urlEncodingType==ENC_QUOTEPRINT)
    {
        // All printable chars but "
        if (  c=='\"' ) return true;
        if (c >= 32 && c<= 126) return false;
    }
    else
    {
        // be strict: Only very safe chars...
        if (c >= 'A' && c<= 'Z') return false;
        if (c >= 'a' && c<= 'z') return false;
        if (c >= '0' && c<= '9') return false;
    }

    return true;
}

size_t Encoders::calcURLEncodingExpandedStringSize(const string &str,const eURLEncodingType & urlEncodingType)
{
    size_t x = 0;
    for (size_t i=0; i<str.size();i++)
    {
        if ( getIfMustBeURLEncoded(str.at(i),urlEncodingType) ) x+=3;
        else x+=1;
    }
    return x;
}
