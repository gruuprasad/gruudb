#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>


template<typename T>
static T ceil_to_pow2(T n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    if constexpr (sizeof(T) >= 2) {
        n |= n >> 8;
        if constexpr (sizeof(T) >= 4) {
            n |= n >> 16;
            if constexpr (sizeof(T) >= 8) {
                n |= n >> 32;
            }
        }
    }
    n++;
    return n;
}

inline uint32_t date_to_int(uint32_t year, uint32_t month, uint32_t day)
{
    return (year << 16) | ((month & 0xFF) << 8) | (day & 0xFF);
}

inline void int_to_date(uint32_t date, uint32_t &year, uint32_t &month, uint32_t &day)
{
    day = date & 0xFF;
    date >>= 8;
    month = date & 0xFF;
    date >>= 8;
    year = date;
}

inline bool streq(const char *first, const char *second) { return 0 == strcmp(first, second); }

struct StrHash
{
    size_t operator()(const char *c_str) const {
        size_t hash = 5381;
        char c;
        while ((c = *c_str++))
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        return hash;
    }
};

struct StrEqual
{
    bool operator()(const char *first, const char *second) const { return streq(first, second); }
};
