#include "dbms/Compression.hpp"
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

#ifdef __linux__
#include <sys/resource.h>
#endif


using namespace dbms;
using namespace std::chrono;


#ifdef __linux__
uint64_t get_memory_reserved()
{
    struct rusage r;
    auto err = getrusage(RUSAGE_SELF, &r);
    return err ? uint64_t(-1) : r.ru_maxrss;
}
#endif


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

    if (argc != 5)
        errx(EXIT_FAILURE, "Usage: %s <NUM_ROWS> <LINEITEM.tbl> <ORDERKEY> <LINENUMBER>", argv[0]);
    const std::size_t num_rows = atoll(argv[1]);
    const char *filename = argv[2];
    const uint32_t O = atoi(argv[3]);
    const uint32_t L = atoi(argv[4]);

    /* Create the column store. */
    ColumnStore *columnstore = new ColumnStore(ColumnStore::Create_Naive(lineitem));
    /* Fill the column store. */
    Loader::load_LineItem(filename, lineitem, *columnstore, num_rows);
#ifdef __linux__
    asm volatile ("" : : : "memory");
    const auto mem_before = get_memory_reserved();
#endif
    /* Create the compressed column store. */
    ColumnStore *compressed_columnstore = compress_columnstore_lineitem(lineitem, *columnstore);
#ifdef __linux__
    asm volatile ("" : : : "memory");
    const auto mem_after = get_memory_reserved();
#endif
    delete columnstore;

#ifdef __linux__
    const auto mem_compressed = mem_after - mem_before;
#else
    const auto mem_compressed = uint64_t(-1);
#endif

    /* Execute the queries. */
    auto Q4 = [=](const ColumnStore &store) { return query::milestone2::Q4(store, O, L); };
    using query::milestone2::Q2;
    using query::milestone2::Q3;
#define BENCHMARK(QUERY, STORE) { \
    auto start = high_resolution_clock::now(); \
    auto result = QUERY(*(STORE)); \
    auto stop = high_resolution_clock::now(); \
    std::cout << "Milestone2, " #QUERY ", " #STORE ", " << result << ", " \
              << duration_cast<nanoseconds>(stop - start).count() / 1e6 << " ms, " \
              << mem_compressed / double(1024) << " MiB" \
              << std::endl; \
}
    BENCHMARK(Q2, compressed_columnstore);
    BENCHMARK(Q3, compressed_columnstore);
    BENCHMARK(Q4, compressed_columnstore);

    delete compressed_columnstore;
}
