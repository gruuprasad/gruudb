#pragma once

#include "dbms/Store.hpp"
#include "dbms/util.hpp"


namespace dbms {

namespace iterator {

template<bool C, typename T>
typename column_iterator<C, T>::reference_type column_iterator<C, T>::operator*() const
{
    return reinterpret_cast<reference_type>(column_);
}

}

template<typename T>
void Column<T>::push_back(T value)
{
    /* TODO 1.3.2 */
    dbms_unreachable("Not implemented.");
}

template<typename T>
Column<T> & ColumnStore::get_column(std::size_t offset)
{
    return *columns_[offset]
}

}
