#include "dbms/Compression.hpp"
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

    ColumnStore columnstore = ColumnStore::Create_Naive(lineitem);
    Loader::load_LineItem(filename, lineitem, columnstore);
    ColumnStore *compressed_columnstore = compress_columnstore_lineitem(lineitem, columnstore);

    std::cout
        << "ColumnStore: " << columnstore.size_in_bytes() / double(1024 * 1024) << " MiB,\n"
        << "Compressed ColumnStore: " << compressed_columnstore->size_in_bytes() / double(1024 * 1024) << " MiB"
        << std::endl;

    delete compressed_columnstore;
}
