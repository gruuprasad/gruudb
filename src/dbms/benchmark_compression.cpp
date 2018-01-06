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
#include <random>
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


bool check_size(const Relation &relation, const ColumnStore *origin, const ColumnStore *compressed)
{
    bool is_equal = true;

    for (unsigned i = 0; i != relation.size(); ++i) {
        const GenericColumn &col_origin     = origin->get_column<int>(i);     // XXX: This works but is non-conforming
        const GenericColumn &col_compressed = compressed->get_column<int>(i); // XXX: This works but is non-conforming
        if (col_origin.size() != col_compressed.size()) {
            is_equal = false;
            std::cerr << "ERROR: columns of attribute \"" << relation[i].name << "\" have different size: expected "
                      << col_origin.size() << ", got " << col_compressed.size() << std::endl;
        }
    }

    return is_equal;
}

bool check_size(const Relation &relation, const ColumnStore *store, const std::size_t num_rows)
{
    bool is_equal = true;

    for (unsigned i = 0; i != relation.size(); ++i) {
        const GenericColumn &col = store->get_column<int>(i); // XXX: This works but is non-conforming
        if (col.size() != num_rows) {
            is_equal = false;
            std::cerr << "ERROR: column of attribute \"" << relation[i].name << "\" has incorrect size: expected "
                      << num_rows << ", got " << col.size() << std::endl;
        }
    }

    return is_equal;
}

void modify_columns(const Relation &relation, ColumnStore *store)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 9);
    int n = dist(rd);

#define ADD(NAME, TYPE, VALUE) store->get_column<TYPE>(relation[NAME].offset()).push_back(VALUE);
    while (n--) {
        ADD("returnflag",    uint8_t,  42);
        ADD("extendedprice", uint64_t, 42);
        ADD("linestatus",    uint8_t,  42);
        ADD("tax",           uint64_t, 42);
        ADD("orderkey",      uint32_t, 42);
        ADD("discount",      uint64_t, 42);
        ADD("shipinstruct",  Char<26>, "DONT CHEAT");
        ADD("partkey",       uint32_t, 42);
        ADD("suppkey",       uint32_t, 42);
        ADD("commitdate",    uint32_t, 42);
        ADD("receiptdate",   uint32_t, 42);
        ADD("shipdate",      uint32_t, 42);
        ADD("linenumber",    uint32_t, 42);
        ADD("shipmode",      Char<11>, "DONT CHEAT");
        ADD("comment",       Char<45>, "DONT CHEAT");
        ADD("quantity",      uint64_t, 42);
    }
#undef ADD
}


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
    std::size_t num_rows = atoll(argv[1]);
    const char *filename = argv[2];
    const uint32_t O = atoi(argv[3]);
    const uint32_t L = atoi(argv[4]);

    /* Create the column store. */
    ColumnStore *columnstore = new ColumnStore(ColumnStore::Create_Naive(lineitem));
    /* Fill the column store. */
    num_rows = Loader::load_LineItem(filename, lineitem, *columnstore, num_rows);
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

    /* Check that the compressed column store actually compresses all columns. */
    if (not check_size(lineitem, columnstore, compressed_columnstore))
        exit(EXIT_FAILURE);

    /* Manipulate the columns of the original store. */
    modify_columns(lineitem, columnstore);

    delete columnstore;

    /* Check that the size of the compressed column store did not change. */
    if (not check_size(lineitem, compressed_columnstore, num_rows))
        exit(EXIT_FAILURE);

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
