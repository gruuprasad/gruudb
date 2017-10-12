#include "dbms/Loader.hpp"
#include "dbms/Schema.hpp"
#include "dbms/Store.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <chrono>
#include <err.h>
#include <iostream>


using namespace dbms;
using namespace std::chrono;


int main(int argc, char **argv)
{
    /* Define the Lineitem relation. */
    Relation lineitem("lineitem", {
            Attribute::Int1("returnflag"),
            Attribute::Int8("extendedprice"),
            Attribute::Int1("linestatus"),
            Attribute::Int8("tax"),
            Attribute::Int4("orderkey"),
            Attribute::Int8("discount"),
            Attribute::Char("shipinstruct", 26),
            Attribute::Int4("partkey"),
            Attribute::Int4("suppkey"),
            Attribute::Int4("commitdate"),
            Attribute::Int4("receiptdate"),
            Attribute::Int4("shipdate"),
            Attribute::Int4("linenumber"),
            Attribute::Char("shipmode", 11),
            Attribute::Char("comment", 45),
            Attribute::Int8("quantity"),
            });

    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: %s <LINEITEM.tbl>", argv[0]);
    const char *filename = argv[1];

    RowStore rowstore       = RowStore::Create_Naive(lineitem);
    ColumnStore columnstore = ColumnStore::Create_Naive(lineitem);

    {
        auto start = high_resolution_clock::now();
        Loader::load_LineItem(filename, lineitem, rowstore);
        auto stop = high_resolution_clock::now();
        std::cout << "RowStore:    "
                  << rowstore.size_in_bytes() / double(1024 * 1024) << " MiB, "
                  << duration_cast<nanoseconds>(stop - start).count() / 1e6 << " ms" << std::endl;
    }
    {
        auto start = high_resolution_clock::now();
        Loader::load_LineItem(filename, lineitem, columnstore);
        auto stop = high_resolution_clock::now();
        std::cout << "ColumnStore: "
                  << columnstore.size_in_bytes() / double(1024 * 1024) << " MiB, "
                  << duration_cast<nanoseconds>(stop - start).count() / 1e6 << " ms" << std::endl;
    }

    std::size_t checksum_rowstore = 0;
    std::size_t checksum_columnstore = 0;

    for (auto it = rowstore.cbegin(), end = rowstore.cend(); it != end; ++it)
        checksum_rowstore += it.get<uint64_t>(1);

    for (auto it = columnstore.get_column<uint64_t>(1).cbegin(), end = columnstore.get_column<uint64_t>(1).cend(); it != end; ++it)
        checksum_columnstore += *it;

    exit(checksum_rowstore == checksum_columnstore ? EXIT_SUCCESS : EXIT_FAILURE);
}
