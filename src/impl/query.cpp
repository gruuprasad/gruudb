#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <unordered_map>
#include <any>


namespace dbms {

namespace query {

namespace milestone1 {

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
    auto end_11 = store.get_column<uint32_t>(11).cbegin();
    auto it_1 = store.get_column<int64_t>(1).cbegin();
    auto it_5 = store.get_column<int64_t>(5).cbegin();
    auto it_3 = store.get_column<int64_t>(3).cbegin();

    while (it_11 != end_11) {
        if ((*it_11) < date_threshold)
            result += (*it_1 * (1 - *it_5) * (1 + *it_3));
        std::cout << *it_11 << "  " << *it_1 << "  " << *it_3 << "  " << *it_5 << "\n";
        ++it_11; ++it_1; ++it_3; ++it_5;
    }

    return result/100;
}

unsigned Q2(const RowStore &store)
{
    unsigned result = 0;

    return result;
}

unsigned Q2(const ColumnStore &store)
{
    unsigned result = 0;

    return result;
}

}

}

}
