#include "dbms/query.hpp"
#include "dbms/Store.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <unordered_map>


namespace dbms {

namespace query {

namespace milestone1 {

uint64_t Q1(const RowStore &store)
{
    /* TODO 1.4.1 */
    dbms_unreachable("Not implemented.");
}

uint64_t Q1(const ColumnStore &store)
{
    /* TODO 1.4.1 */
    dbms_unreachable("Not implemented.");
}

unsigned Q2(const RowStore &store)
{
    /* TODO 1.4.1 */
    dbms_unreachable("Not implemented.");
}

unsigned Q2(const ColumnStore &store)
{
    /* TODO 1.4.1 */
    dbms_unreachable("Not implemented.");
}

}

namespace milestone2 {

unsigned Q2(const ColumnStore &store)
{
    /* TODO 2.2.2 */
    dbms_unreachable("Not implemented.");
}

unsigned Q3(const ColumnStore &store)
{
    /* TODO 2.2.3 */
    dbms_unreachable("Not implemented.");
}

unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L)
{
    /* TODO 2.2.4 */
    dbms_unreachable("Not implemented.");
}

}

namespace milestone3 {

unsigned Q3(const ColumnStore &store, shipdate_index_type &index)
{
    /* TODO 3.3.1 */
    dbms_unreachable("Not implemented.");
}

unsigned Q4(const ColumnStore &store, uint32_t O, uint32_t L, primary_index_type &index)
{
    /* TODO 3.3.2 */
    dbms_unreachable("Not implemented.");
}

unsigned Q5(const ColumnStore &lineitem, const ColumnStore &orders)
{
    /* TODO 3.3.3 */
    dbms_unreachable("Not implemented.");
}

}

}

}
