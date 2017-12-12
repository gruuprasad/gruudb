#include "dbms/assert.hpp"
#include "dbms/Compression.hpp"
#include <cstdint>


namespace dbms {

/**
 * This class implements a generic dictionary.
 * It keeps an internal bi-directional mapping from input value to dictionary index.
 */
template<typename T, typename S>
struct Dictionary
{
    using value_type = T;
    using index_type = S;

    /** Returns the size of the dictionary, i.e. the number of distinct values. */
    std::size_t size() const { /* TODO 2.2.2 */ dbms_unreachable("Not implemented."); }

    /** Returns the index of the given value in the dictionary. */
    index_type operator()(T value) {
        /* TODO 2.2.2 */
        dbms_unreachable("Not implemented.");
    }

    /** Returns the value at the given index in the dictionary. */
    const T & operator[](index_type idx) const {
        /* TODO 2.2.2 */
        dbms_unreachable("Not implemented.");
    }

    friend std::ostream & operator<<(std::ostream &out, const Dictionary<T> &dict) {
        dbms_unreachable("Not implemented.");
    }
    DECLARE_DUMP

    private:
    /* TODO 2.2.2 */
};

template<typename T>
void Column<RLE<T>>::push_back(T value)
{
    /* TODO 2.2.1 */
    dbms_unreachable("Not implemented.");
}

template<typename T, typename S>
void Column<Dictionary<T, S>>::push_back(T value)
{
    /* TODO 2.2.2 */
    dbms_unreachable("Not implemented.");
}

template<typename T, typename S>
void Column<RLE<Dictionary<T, S>>>::push_back(T value)
{
    /* TODO 2.2.3 */
    dbms_unreachable("Not implemented.");
}

}
