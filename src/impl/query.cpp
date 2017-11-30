#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
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
  float result = 0;

  for (auto it = store.cbegin(), end = store.cend(); it != end; ++it) {
      if (it.get<uint32_t>(11) < date_threshold)  {
          result += (float)it.get<int64_t>(1)/100 * (1 - (float)it.get<int64_t>(5)/100) * (1 + (float)it.get<int64_t>(3)/100);
          /* std::cout <<  result << "  " << it.get<int64_t>(1) << "  " << it.get<int64_t>(5) << "  "  <<  it.get<int64_t>(3) << std::endl; */
      }
  }

  return (uint64_t)result;
}

uint64_t Q1(const ColumnStore &store)
{
    const uint32_t date_threshold = date_to_int(1998, 1, 1);
    float result = 0;

    auto it_11 = store.get_column<uint32_t>(11).cbegin();
    auto end_11 = store.get_column<uint32_t>(11).cend();
    auto it_1 = store.get_column<int64_t>(1).cbegin();
    auto it_5 = store.get_column<int64_t>(5).cbegin();
    auto it_3 = store.get_column<int64_t>(3).cbegin();
    
    for (; it_11 != end_11; ++it_11, ++it_1, ++it_3, ++it_5) {
        if (*it_11 < date_threshold)
            result += ((float)*it_1 / 100 * (1 - (float)*it_5 / 100) * (1 + (float)*it_3 / 100));
    }

    return result;
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

}

}
