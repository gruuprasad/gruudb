#include "dbms/Loader.hpp"
#include "dbms/query.hpp"
#include "dbms/Schema.hpp"
#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/RowStore.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <err.h>
#include <iostream>
#include <string>
#include <unordered_map>


using namespace dbms;
using namespace std::chrono;


int main(int argc, char **argv)
{
    Relation lineitem("lineitem", {
            /* 00 */ Attribute::Int1("returnflag"),
            /* 01 */ Attribute::Int8("extendedprice"),
            /* 02 */ Attribute::Int1("linestatus"),
            /* 03 */ Attribute::Int8("tax"),
            /* 04 */ Attribute::Int4("orderkey"),
            /* 05 */ Attribute::Int8("discount"),
            /* 06 */ Attribute::Char("shipinstruct", 26),
            /* 07 */ Attribute::Int4("partkey"),
            /* 08 */ Attribute::Int4("suppkey"),
            /* 09 */ Attribute::Int4("commitdate"),
            /* 10 */ Attribute::Int4("receiptdate"),
            /* 11 */ Attribute::Int4("shipdate"),
            /* 12 */ Attribute::Int4("linenumber"),
            /* 13 */ Attribute::Char("shipmode", 11),
            /* 14 */ Attribute::Char("comment", 45),
            /* 15 */ Attribute::Int8("quantity"),
            });

    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: %s <NUM_ROWS> <LINEITEM.tbl>", argv[0]);
    const std::size_t num_rows = atoll(argv[1]);
    const char *filename = argv[2];

    /* Create the stores. */
    RowStore rowstore = RowStore::Create_Optimized(lineitem);
    ColumnStore columnstore = ColumnStore::Create_Naive(lineitem);

    /* Fill the stores. */
    Loader::load_LineItem(filename, lineitem, rowstore, num_rows);
    Loader::load_LineItem(filename, lineitem, columnstore, num_rows);

    /* Execute the queries. */
#define BENCHMARK(QUERY, STORE) { \
    auto start = high_resolution_clock::now(); \
    auto result = query::milestone1::QUERY(STORE); \
    auto stop = high_resolution_clock::now(); \
    std::cout << "Milestone1, " #QUERY ", " #STORE ", " << result << ", " << duration_cast<nanoseconds>(stop - start).count() / 1e6 << " ms" << std::endl; \
}
    BENCHMARK(Q1, rowstore);
    BENCHMARK(Q1, columnstore);
    BENCHMARK(Q2, rowstore);
    BENCHMARK(Q2, columnstore);
}
