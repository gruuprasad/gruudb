/*--- macros.hpp -------------------------------------------------------------------------------------------------------
 *
 * This file provides convenience macros.
 *
 *--------------------------------------------------------------------------------------------------------------------*/


#pragma once

#define ID(X) X
#define STR(X) STR_(X)
#define STR_(X) #X

#define DECLARE_ENUM(LIST) \
    enum LIST { \
        LIST(ID) \
    }
#define ENUM_TO_STR(LIST) LIST(STR)

#define ARR_SIZE(ARR) (sizeof(ARR) / sizeof(*(ARR)))

#define DECLARE_DUMP \
    void dump(std::ostream &out) const __attribute__((noinline)) { out << *this << std::endl; } \
    void dump() const __attribute__((noinline)) { dump(std::cerr); }
#define DECLARE_DUMP_VIRTUAL \
    virtual void dump(std::ostream &out) const __attribute__((noinline)) { out << *this << std::endl; } \
    virtual void dump() const __attribute__((noinline)) { dump(std::cerr); }
