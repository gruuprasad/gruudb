#include "dbms/assert.hpp"
#include "dbms/Compression.hpp"
#include <cstdint>
#include <map>


namespace dbms {

/**
 * This class implements a generic dictionary.
 * It keeps an internal bi-directional mapping from input value to dictionary index.
 */
template<typename T>
struct Dictionary
{
    using value_type = T;
    using index_type = uint32_t;

    /** Returns the size of the dictionary, i.e. the number of distinct values. */
    std::size_t size() const { return dictionary_table.size(); }

    /** Returns the index of the given value in the dictionary. */
    index_type operator()(T value) {
        for (auto it : dictionary_table) { 
            if(it.second == value)
				return it.first;
        }
        dictionary_table[dictionary_table.size() + 1] = value;
        return dictionary_table.size() + 1;
    }

    /** Returns the value at the given index in the dictionary. */
    const T & operator[](index_type idx) const {
        return dictionary_table.at(idx);
    }

    friend std::ostream & operator<<(std::ostream &out, const Dictionary<T> &dict) {
        dbms_unreachable("Not implemented.");
    }
    DECLARE_DUMP

    private:
    std::map<index_type, value_type> dictionary_table;
};

template<typename T>
void Column<RLE<T>>::push_back(T value)
{
    if (size_ == capacity_)
        reserve(size_ + capacity_ + capacity_ / 2);
    auto it = runs_end();
    if (size_ != 0 && (--it)->value == value) 
        it->count++;
    else {
        new (run_iterator(*this, size_).operator->()) RLE<T>(value);
        size_++;
    }
    num_rows_++;
}

template<typename T>
void Column<Dictionary<T>>::push_back(T value)
{
    /* TODO 2.2.2 */
    dbms_unreachable("Not implemented.");
}

template<typename T>
void Column<RLE<Dictionary<T>>>::push_back(T value)
{
    /* TODO 2.2.3 */
    dbms_unreachable("Not implemented.");
}

}
