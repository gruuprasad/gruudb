#include "impl/RowStore.hpp"
#include <algorithm>


using namespace dbms;


std::size_t get_alignment_requirement(const Attribute &attr)
{
    switch (attr.type) {
        case Attribute::TY_Int:
        case Attribute::TY_Float:
        case Attribute::TY_Double:
            return attr.size;

        case Attribute::TY_Char:
            return 1;

        case Attribute::TY_Varchar:
            return sizeof(Varchar);

        default: dbms_unreachable("unknown attribute type");
    }
}

RowStore::~RowStore()
{
    free(data_);
    free(offsets_);
}

RowStore RowStore::Create_Naive(const Relation &relation)
{
    RowStore row_store;

    row_store.num_attributes_ = relation.size();
    row_store.offsets_ = static_cast<decltype(offsets_)>(malloc(relation.size() * sizeof(*offsets_)));

    /* Compute the required size of a row.  Remember that there can be padding. */
    std::size_t row_size = 0;
    std::size_t max_alignment = 0;

    for (auto attr : relation) {
        std::size_t elem_size;
        std::size_t elem_alignment;
        switch (attr.type) {
            case Attribute::TY_Int:
            case Attribute::TY_Float:
            case Attribute::TY_Double:
                elem_size = elem_alignment = attr.size;
                break;

            case Attribute::TY_Char:
                elem_size = attr.size;
                elem_alignment = 1;
                break;

            case Attribute::TY_Varchar:
                elem_size = elem_alignment = sizeof(void*);
                break;

            default: dbms_unreachable("unknown attribute type");
        }

        max_alignment = std::max(elem_alignment, max_alignment);
        if (row_size % elem_alignment) { // add padding bytes for alignment
            std::size_t padding = elem_alignment - row_size % elem_alignment;
            row_size += padding;
        }
        row_store.offsets_[attr.offset()] = row_size;
        row_size += elem_size;
    }
    if (row_size % max_alignment) row_size += max_alignment - row_size % max_alignment;
    row_store.row_size_ = row_size;

    return row_store;
}

bool sortbysecond(const std::pair<int, std::size_t> &a, const std::pair<int, std::size_t> &b)
{
    return(a.second<b.second);
}

RowStore RowStore::Create_Optimized(const Relation &relation)
{
    RowStore row_store;
    
    std::vector<std::size_t> order;
    std::vector<std::pair <int, std::size_t>> vect;
   
    for(std::size_t i =0; i < relation.size(); i++) {
        vect.push_back(std::make_pair(i, relation[i].size));
    }
    
    std::sort(vect.begin(), vect.end(), sortbysecond);

    for(std::size_t i =0; i < relation.size(); i++) {
        order.push_back(vect[i].first);
    }
    
    row_store.num_attributes_ = relation.size();
    row_store.offsets_ = static_cast<decltype(offsets_)>(malloc(relation.size() * sizeof(*offsets_)));

    /* Compute the required size of a row.  Remember that there can be padding. */
    std::size_t row_size = 0;
    std::size_t max_alignment = 0;

    for (std::size_t i = 0; i != relation.size(); ++i) {
        auto attr = relation[order[i]];
        std::size_t elem_size;
        std::size_t elem_alignment;
        switch (attr.type) {
            case Attribute::TY_Int:
            case Attribute::TY_Float:
            case Attribute::TY_Double:
                elem_size = elem_alignment = attr.size;
                break;

            case Attribute::TY_Char:
                elem_size = attr.size;
                elem_alignment = 1;
                break;

            case Attribute::TY_Varchar:
                elem_size = elem_alignment = sizeof(void*);
                break;

            default: dbms_unreachable("unknown attribute type");
        }

        max_alignment = std::max(elem_alignment, max_alignment);
            
        if (row_size % elem_alignment) { // add padding bytes for alignment
            std::size_t padding = elem_alignment - row_size % elem_alignment;
            row_size += padding;
        }
        row_store.offsets_[attr.offset()] = row_size;
        row_size += elem_size;
    }
    if (row_size % max_alignment) row_size += max_alignment - row_size % max_alignment;
    row_store.row_size_ = row_size;
    
    return row_store;
}

RowStore RowStore::Create_Explicit(const Relation &relation, std::size_t *order)
{
    RowStore row_store;

    row_store.num_attributes_ = relation.size();
    row_store.offsets_ = static_cast<decltype(offsets_)>(malloc(relation.size() * sizeof(*offsets_)));

    /* Compute the required size of a row.  Remember that there can be padding. */
    std::size_t row_size = 0;
    std::size_t max_alignment = 0;

    for (std::size_t i = 0; i != relation.size(); ++i) {
        auto attr = relation[order[i]];
        std::size_t elem_size;
        std::size_t elem_alignment;
        switch (attr.type) {
            case Attribute::TY_Int:
            case Attribute::TY_Float:
            case Attribute::TY_Double:
                elem_size = elem_alignment = attr.size;
                break;

            case Attribute::TY_Char:
                elem_size = attr.size;
                elem_alignment = 1;
                break;

            case Attribute::TY_Varchar:
                elem_size = elem_alignment = sizeof(void*);
                break;

            default: dbms_unreachable("unknown attribute type");
        }

        max_alignment = std::max(elem_alignment, max_alignment);
            
        if (row_size % elem_alignment) { // add padding bytes for alignment
            std::size_t padding = elem_alignment - row_size % elem_alignment;
            row_size += padding;
        }
        row_store.offsets_[attr.offset()] = row_size;
        row_size += elem_size;
    }
    if (row_size % max_alignment) row_size += max_alignment - row_size % max_alignment;
    row_store.row_size_ = row_size;

    return row_store;
}


void RowStore::reserve(std::size_t new_cap)
{
    if (new_cap > capacity()) {
        void *new_data_ = realloc(data_, row_size_ * new_cap);
        if (new_data_ != NULL) {
            data_ = new_data_;
            capacity_ = new_cap;
        }
    //TODO handle memory allocation failure
    }
}

RowStore::iterator RowStore::append(std::size_t n_rows)
{
    size_ += n_rows;
    if (size_ > capacity_) {
        reserve(capacity_ + std::max(n_rows, capacity_ + (capacity_ / 2)));
    } 
    return iterator(*this, size_ - n_rows);
}
