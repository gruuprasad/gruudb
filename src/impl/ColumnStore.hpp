#pragma once

#include "dbms/Store.hpp"
#include "dbms/util.hpp"


namespace dbms {

namespace iterator {

template<bool C, typename T>
typename column_iterator<C, T>::reference_type column_iterator<C, T>::operator*() const
{
  return reinterpret_cast<column_iterator<C, T>::reference_type>(static_cast<uint8_t *>(column_.data_)[column_.elem_size() * idx_]);
}

}

template<typename T>
void Column<T>::push_back(T value)
{
    if (size() == capacity())
      reserve(size() + elem_size() * 25);
    *iterator(*this, size()) = value;
    size_++;
}

template<typename T>
Column<T> & ColumnStore::get_column(std::size_t offset)
{
    return *static_cast<Column<T>*>(columns_[offset]); 
}

}
