//
// Created by frank on 17-12-28.
//

#include <type_traits>

#include <jackson/Writer.h>

using namespace json;

namespace
{

unsigned countDigits(uint32_t n)
{
    static const uint32_t powers_of_10[] = {
            0,
            10,
            100,
            1000,
            10000,
            100000,
            1000000,
            10000000,
            100000000,
            1000000000
    };
    //
    // about magic number:
    //     http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
    // __builtin_clz is gcc builtin:
    //     https://en.wikipedia.org/wiki/Bit_Manipulation_Instruction_Sets
    //     https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
    //
    uint32_t t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
    return t - (n < powers_of_10[t]) + 1;
}

unsigned countDigits(uint64_t n)
{
    static const uint64_t powers_of_10[] = {
            0,
            10,
            100,
            1000,
            10000,
            100000,
            1000000,
            10000000,
            100000000,
            1000000000,
            10000000000,
            100000000000,
            1000000000000,
            10000000000000,
            100000000000000,
            1000000000000000,
            10000000000000000,
            100000000000000000,
            1000000000000000000,
            10000000000000000000U
    };
    uint32_t t = (64 - __builtin_clzll(n | 1)) * 1233 >> 12;
    return t - (n < powers_of_10[t]) + 1;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

template <typename T>
unsigned itoa_(T val, char* buf)
{
    static_assert(std::is_unsigned<T>::value, "must be unsigned integer");

    static const char digits[201] =
            "0001020304050607080910111213141516171819"
            "2021222324252627282930313233343536373839"
            "4041424344454647484950515253545556575859"
            "6061626364656667686970717273747576777879"
            "8081828384858687888990919293949596979899";

    unsigned count = countDigits(val);
    unsigned next = count - 1;

    while (val >= 100) {
        unsigned i = (val % 100) * 2;
        val /= 100;
        buf[next] = digits[i + 1];
        buf[next - 1] = digits[i];
        next -= 2;
    }

    /* Handle last 1-2 digits. */
    if (val < 10) {
        buf[next] = '0' + val;
    }
    else {
        unsigned i = val * 2;
        buf[next] = digits[i + 1];
        buf[next - 1] = digits[i];
    }
    return count;
}
#pragma GCC diagnostic pop


}

namespace json::detail
{


unsigned itoa(int32_t val, char* buf)
{
    auto u = static_cast<uint32_t>(val);
    if (val < 0) {
        *buf++ = '-';
        u = ~u + 1;
    }
    return (val < 0) + itoa_(u, buf);
}


unsigned itoa(int64_t val, char* buf)
{
    auto u = static_cast<uint64_t>(val);
    if (val < 0) {
        *buf++ = '-';
        u = ~u + 1;
    }
    return (val < 0) + itoa_(u, buf);
}


}