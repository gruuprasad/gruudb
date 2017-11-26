#pragma once

#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include <algorithm>
#include <vector>


namespace dbms {

const std::size_t ROW_CHUNK = 2400; // minimum number of rows allocated during one reserve

namespace iterator {

template<bool C>
template<typename T>
std::conditional_t<rowstore_iterator<C>::is_const, const T&, T&>
rowstore_iterator<C>::get(std::size_t offset) const
{
    return reinterpret_cast<std::conditional_t<rowstore_iterator<C>::is_const, const T&, T&>>(row_[store_.offsets_[offset]]);
}

}

}
