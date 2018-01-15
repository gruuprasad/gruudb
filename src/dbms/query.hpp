#pragma once

#include "dbms/Store.hpp"
#include <cstdint>
#include <functional>


namespace dbms {

/* Define index data structures. */
template<typename Key, typename Value, typename Compare>
struct BPlusTree;

template<typename Key, typename Value, typename Hash, typename KeyEqual>
struct hash_map;

using primary_key_type = std::pair<uint32_t, uint32_t>;
struct hash
{
    std::size_t operator()(const dbms::primary_key_type &key) const {
        uint64_t v = uint64_t(key.first) << 32 | key.second;
        return Murmur3{}(v);
    }
};
struct equal
{
    bool operator()(const dbms::primary_key_type &first, const dbms::primary_key_type &second) const {
        return first.first == second.first and first.second == second.second;
    }
};

using primary_index_type = hash_map<primary_key_type, const Char<45>*, hash, equal>;
using shipdate_index_type = BPlusTree<uint32_t, uint64_t, std::less<uint32_t>>;


namespace query {

namespace milestone1 {

uint64_t Q1(const RowStore &store);
uint64_t Q1(const ColumnStore &store);
unsigned Q2(const RowStore &store);
unsigned Q2(const ColumnStore &store);

}

namespace milestone2 {

unsigned Q2(const ColumnStore &store);
unsigned Q3(const ColumnStore &store);
unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L);

}

namespace milestone3 {

unsigned Q3(const ColumnStore &store, shipdate_index_type &index);
unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L, primary_index_type &index);
unsigned Q5(const ColumnStore &lineitem, const ColumnStore &orders);

}

}

}
