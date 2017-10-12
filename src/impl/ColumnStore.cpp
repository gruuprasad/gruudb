#include "impl/ColumnStore.hpp"


using namespace dbms;


void GenericColumn::reserve(std::size_t new_cap)
{
    /* TODO 1.3.1 */
    dbms_unreachable("Not implemented.");
}

ColumnStore::~ColumnStore()
{
    /* TODO 1.3.1 */
    dbms_unreachable("Not implemented.");
}

ColumnStore ColumnStore::Create_Naive(const Relation &relation)
{
    /* TODO 1.3.1 */
    dbms_unreachable("Not implemented.");
}

ColumnStore ColumnStore::Create_Explicit(std::initializer_list<ColumnBase*> columns)
{
    /* TODO 1.3.3 (?) */
    dbms_unreachable("Not implemented.");
}

std::size_t ColumnStore::size_in_bytes() const
{
    /* TODO 1.3.1 */
    dbms_unreachable("Not implemented.");
}

std::size_t ColumnStore::capacity_in_bytes() const
{
    /* TODO 1.3.1 */
    dbms_unreachable("Not implemented.");
}
