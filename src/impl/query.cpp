#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <unordered_map>
#include <chrono>


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
  uint64_t result = 0;

  for (auto it = store.cbegin(), end = store.cend(); it != end; ++it) {
      if (it.get<uint32_t>(11) < date_threshold) 
          result += it.get<int64_t>(1) * (1 - it.get<int64_t>(5)) * (1 + it.get<int64_t>(3));
  }

  return result/100;
}

uint64_t Q1(const ColumnStore &store)
{
    const uint32_t date_threshold = date_to_int(1998, 1, 1);
    uint64_t result = 0;

    auto it_11 = store.get_column<uint32_t>(11).cbegin();
    auto end_11 = store.get_column<uint32_t>(11).cend();
    auto it_1 = store.get_column<int64_t>(1).cbegin();
    auto it_5 = store.get_column<int64_t>(5).cbegin();
    auto it_3 = store.get_column<int64_t>(3).cbegin();

    while (it_11 != end_11) {
        if ((*it_11) < date_threshold)
            result += (*it_1 * (1 - *it_5) * (1 + *it_3));
        ++it_11; ++it_1; ++it_3; ++it_5;
    }

    return result/100;
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
    
    std::unordered_map<std::size_t, unsigned> mode_count;

    std::hash<Char<11>> hashChar;
    
    for (auto it = store.cbegin(), end = store.cend(); it != end; ++it) {
       ++mode_count[hashChar(it.get<Char<11>>(13))];
    }

    for (auto it : mode_count)
        result = std::max(result, it.second);

    return result;
}

unsigned Q2(const ColumnStore &store)
{
    unsigned result = 0;
    std::hash<Char<11>> hashChar;
    std::unordered_map<std::size_t, unsigned> mode_count;
    auto it_13 = store.get_column<Char<11>>(13).cbegin();
    auto end_13 = store.get_column<Char<11>>(13).cend();

    while (it_13 != end_13) {
       ++mode_count[hashChar(*it_13)];
       ++it_13;
    }
    
    for (auto it : mode_count)
        result = std::max(result, it.second);

    return result;
}

}

}

}
