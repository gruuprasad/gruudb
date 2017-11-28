#include "impl/ColumnStore.hpp"


using namespace dbms;


void GenericColumn::reserve(std::size_t new_cap)
{
    if (new_cap > capacity()) {
        void *new_data_ = realloc(data_, elem_size_ * new_cap);
        if (new_data_ != NULL) {
            data_ = new_data_;
            capacity_ = new_cap;
        }
        //TODO handle memory allocation failure -(alternate) use malloc and handle memory chunks
    }
}

ColumnStore::~ColumnStore()
{
    for (auto column_ : columns_)
        delete column_;
}

ColumnStore ColumnStore::Create_Naive(const Relation &relation)
{
    ColumnStore column_store;

    for (auto attr: relation) { 
      if (attr.type == Attribute::TY_Varchar)
          column_store.columns_.push_back(new GenericColumn(sizeof(void*)));
      else 
          column_store.columns_.push_back(new GenericColumn(attr.size));
    }

    return column_store;
}

ColumnStore ColumnStore::Create_Explicit(std::initializer_list<ColumnBase*> columns)
{
    /* TODO 1.3.3 (?) */
    dbms_unreachable("Not implemented.");
}

std::size_t ColumnStore::size_in_bytes() const
{
    std::size_t size_bytes = 0;

    for(auto column : columns_)
        size_bytes += column->size_in_bytes();
    
    return size_bytes;
}

std::size_t ColumnStore::capacity_in_bytes() const
{
    std::size_t capacity_bytes = 0;

    for (auto column : columns_)
        capacity_bytes += column->capacity_in_bytes();

    return capacity_bytes;
}
