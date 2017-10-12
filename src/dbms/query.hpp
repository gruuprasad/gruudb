#pragma once

#include "dbms/Store.hpp"
#include <cstdint>


namespace dbms {

namespace query {

namespace milestone1 {

uint64_t Q1(const RowStore &store);
uint64_t Q1(const ColumnStore &store);
unsigned Q2(const RowStore &store);
unsigned Q2(const ColumnStore &store);

}

}

}
