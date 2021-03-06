#include "dbms/query.hpp"
#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/Compression.hpp"
#include "impl/RowStore.hpp"
#include "impl/BPlusTree.hpp"
#include "impl/HashTable.hpp"
#include <unordered_map>
#include <chrono>

using namespace std::chrono;

namespace dbms {

namespace query {

namespace milestone1 {

/* Query 1
 * SELECT CAST(SUM( extendedprice * (1 - discount) * (1 + tax)) AS INT)
 * FROM lineitem
 * WHERE shipdate < DATE('1998-01-01')
 */

uint64_t Q1(const RowStore &store)
{
  const uint32_t date_threshold = date_to_int(1998, 1, 1);
  int64_t result = 0;

  for (auto it = store.cbegin(), end = store.cend(); it != end; ++it) {
      if (it.get<uint32_t>(11) < date_threshold)  {
          result += (it.get<int64_t>(1) * (100 - it.get<int64_t>(5)) * (100 + it.get<int64_t>(3)));
      }
  }

  return result/1000000;
}

uint64_t Q1(const ColumnStore &store)
{
    const uint32_t date_threshold = date_to_int(1998, 1, 1);
    int64_t result = 0;

    auto it_11 = store.get_column<uint32_t>(11).cbegin();
    auto end_11 = store.get_column<uint32_t>(11).cend();
    auto it_1 = store.get_column<int64_t>(1).cbegin();
    auto it_5 = store.get_column<int64_t>(5).cbegin();
    auto it_3 = store.get_column<int64_t>(3).cbegin();
    
    for (; it_11 != end_11; ++it_11, ++it_1, ++it_3, ++it_5) {
        if (*it_11 < date_threshold)
            result += *it_1  * (100 - *it_5) * (100 + *it_3);
    }

    return result/1000000;
}

/*
 * Query 2
 * SELECT COUNT(*) as num
 * FROM lineitem
 * GROUP BY shipmode
 * ORDER BY num DESC
 * LIMIT 1;
 */

unsigned Q2(const RowStore &store)
{
    unsigned result = 0;
    
    std::vector<unsigned> mode_count{0, 0, 0, 0, 0, 0, 0};

    for (auto it = store.cbegin(), end = store.cend(); it != end; ++it) {
        switch (static_cast<char>((it.get<Char<11> >(13)).data[0])) {
            case 'T':
                ++mode_count[0];
                break;
            case 'M':
                ++mode_count[1];
                break;
            case 'A':
                ++mode_count[2];
                break;
            case 'F':
                ++mode_count[3];
                break;
            case 'S':
                ++mode_count[4];
                break;
            case 'R':
                if (static_cast<char>(it.get<Char<11>>(13).data[1])  == 'E')
                    ++mode_count[5];
                else
                    ++mode_count[6];
                break;
            default:
                std::cout << it.get<Char<11>>(13) << std::endl;
                exit(1);
        }
    }

    for (auto it : mode_count)
        result = std::max(result, it);

    return result;
}

unsigned Q2(const ColumnStore &store)
{
    unsigned result = 0;
    std::vector<unsigned> mode_count{0, 0, 0, 0, 0, 0, 0};
    auto it_13 = store.get_column<Char<11>>(13).cbegin();
    auto end_13 = store.get_column<Char<11>>(13).cend();

    while (it_13 != end_13) {
       switch (static_cast<char>((*it_13).data[0])) {
            case 'T':
                ++mode_count[0];
                break;
            case 'M':
                ++mode_count[1];
                break;
            case 'A':
                ++mode_count[2];
                break;
            case 'F':
                ++mode_count[3];
                break;
            case 'S':
                ++mode_count[4];
                break;
            case 'R':
                if (static_cast<char>((*it_13).data[1])  == 'E')
                    ++mode_count[5];
                else
                    ++mode_count[6];
                break;
            default:
                std::cout << *it_13 << std::endl;
                exit(1);
        }
       ++it_13;
    }
    
    for (auto it : mode_count)
        result = std::max(result, it);

    return result;
}

}

namespace milestone2 {

unsigned Q2(const ColumnStore &store)
{
    unsigned result = 0;
    std::vector<unsigned> mode_count{0, 0, 0, 0, 0, 0, 0};
    auto it_13 = store.get_column<RLE<Char<11>>>(13).runs_begin();
    auto end_13 = store.get_column<RLE<Char<11>>>(13).runs_end();

    while (it_13 != end_13) {
       switch (static_cast<char>((it_13->value).data[0])) {
            case 'T':
                mode_count[0] += it_13->count;
                break;
            case 'M':
                mode_count[1] += it_13->count;
                break;
            case 'A':
                mode_count[2] += it_13->count;
                break;
            case 'F':
                mode_count[3] += it_13->count;
                break;
            case 'S':
                mode_count[4] += it_13->count;
                break;
            case 'R':
                if (static_cast<char>((it_13->value).data[1])  == 'E')
                    mode_count[5] += it_13->count;
                else
                    mode_count[6] += it_13->count;
                break;
            default:
                std::cout << *it_13 << std::endl;
                exit(1);
        }
       ++it_13;
    }
    
    for (auto it : mode_count)
        result = std::max(result, it);

    return result;
}

unsigned Q3(const ColumnStore &store)
{
    const uint32_t start_date = date_to_int(1993, 1, 1);
    const uint32_t end_date = date_to_int(1993, 31, 12);

    unsigned result = 0;

    auto it_11 = store.get_column<RLE<uint32_t>>(11).cbegin();
    auto end_11 = store.get_column<RLE<uint32_t>>(11).cend();
    auto it_15 = store.get_column<RLE<uint64_t>>(15).cbegin();
    
    while (it_11 != end_11) {
        if (*it_11 >= start_date && *it_11 <= end_date)
            result += *it_15;
        ++it_11; ++it_15;
    }
    
    return result;
 
}

unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L)
{
    unsigned result = 0;

    auto it_4 = store.get_column<RLE<uint32_t>>(4).cbegin();
    auto end_4 = store.get_column<RLE<uint32_t>>(4).cend();
    auto it_12 = store.get_column<RLE<uint32_t>>(12).cbegin();
    auto it_14 = store.get_column<Char<45>>(14).cbegin();

    std::size_t store_size = store.size();

    for (std::size_t i = 0; i < store_size; ++i) {
        if (*it_4 == O && *it_12 == L)
            return (unsigned)strlen(*it_14);
        ++it_4; ++it_12; ++it_14;
    }
    return result;
}

}

namespace milestone3 {

unsigned Q3(const ColumnStore &store, shipdate_index_type &index)
{
    
    const uint32_t start_date = date_to_int(1993, 1, 1);
    const uint32_t end_date = date_to_int(1994, 1, 1);

    unsigned result = 0;
    
    auto range = index.in_range(start_date,end_date);
    for(auto it= range.begin(); it!=range.end(); ++it)
    {
        result += (*it).second;
    }
    return result;
}

unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L, primary_index_type &index)
{
    auto it = index.find(std::make_pair(O, L));
    if (it != index.end())
        return (unsigned)strlen(*((*it).second));
    else
        return 0;
}

unsigned Q5(const ColumnStore &lineitem, const ColumnStore &orders)
{
    auto tax_it = lineitem.get_column<RLE<int64_t>>(3).cbegin();
    auto extendPrice_it = lineitem.get_column<RLE<int64_t>>(1).cbegin();
    auto order_key_it = lineitem.get_column<RLE<uint32_t>>(4).cbegin();
    auto shipmode_it = lineitem.get_column<RLE<Char<11>>>(13).cbegin();
    auto order_key_end = lineitem.get_column<RLE<uint32_t>>(4).cend();
    auto ordersStore_O_it = orders.get_column<uint32_t>(1).cbegin();
    auto ordersStore_O_end = orders.get_column<uint32_t>(1).cend();
    auto ordersStore_status = orders.get_column<uint8_t>(8).cbegin();

    std::unordered_map<uint32_t, double> key_index{};
    for (std::size_t i = 0; i < lineitem.size(); ++i) {
        if (strcmp((const char*) *shipmode_it, "AIR") == 0)
            key_index[*order_key_it] += (*extendPrice_it) * (*tax_it);
        ++order_key_it; ++shipmode_it; ++extendPrice_it; ++tax_it;
    }
    
    std::pair<uint32_t, double> max{0, 0};
    std::size_t orders_size = orders.size();
    for (std::size_t i = 0; i < orders_size; ++i) {
        if (*ordersStore_status == 'F') {
            auto lookup = key_index.find(*ordersStore_O_it);
            if (lookup != key_index.end() && lookup->second > max.second)
                max = *lookup;
        }
        ++ordersStore_O_it; ++ordersStore_status;
    }

    return max.first;
}

}

}

}
