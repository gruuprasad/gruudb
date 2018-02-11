#include "dbms/assert.hpp"
#include "dbms/Loader.hpp"
#include "dbms/query.hpp"
#include "dbms/Schema.hpp"
#include "dbms/Store.hpp"
#include "dbms/util.hpp"
#include "impl/BPlusTree.hpp"
#include "impl/ColumnStore.hpp"
#include "impl/Compression.hpp"
#include "impl/HashTable.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <err.h>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

#ifdef __linux__
#include <sys/resource.h>
#endif


using namespace dbms;
using namespace std::chrono;


uint64_t get_memory_reserved()
{
#ifdef __linux__
    struct rusage r;
    auto err = getrusage(RUSAGE_SELF, &r);
    return err ? uint64_t(-1) : r.ru_maxrss;
#else
    return 0;
#endif
}

struct chunk
{
    union
    {
        chunk *next = nullptr;
        uint8_t __chunk[512];
    };
};

chunk * exhaust_reserved_memory(chunk *head = nullptr)
{
    const auto mem_current = get_memory_reserved();
    do {
        chunk *c = new chunk();
        c->next = head;
        head = c;
    } while (get_memory_reserved() - 10 <= mem_current);
    return head;
}

shipdate_index_type build_shipdate_index(const Relation &relation, const ColumnStore &store)
{
    auto &shipdate_col = store.get_column<uint32_t>(relation["shipdate"].offset());
    auto &quantity_col = store.get_column<uint64_t>(relation["quantity"].offset());

    using entry_type = std::pair<uint32_t, uint64_t>;
    std::vector<entry_type> data;
    data.reserve(shipdate_col.size());

    auto quantity_it = quantity_col.begin();
    for (auto shipdate : shipdate_col)
        data.push_back({shipdate, *quantity_it++});

    struct Compare
    {
        bool operator()(const entry_type &first, const entry_type &second) const {
            return first.first < second.first;
        }
    };

    std::sort(data.begin(), data.end(), Compare{});

    asm volatile ("" : : : "memory");
    const auto mem_before = get_memory_reserved();
    const auto time_begin = high_resolution_clock::now();

    auto tree = shipdate_index_type::Bulkload(data.begin(), data.end());

    asm volatile ("" : : : "memory");
    const auto time_end = high_resolution_clock::now();
    const auto mem_after = get_memory_reserved();

    std::cout << "Milestone3, Q3, bulkload, "
              << data.size() << ", "
              << duration_cast<nanoseconds>(time_end - time_begin).count() / 1e6 << " ms, "
              << (mem_after - mem_before) / 1024.f << " MiB\n";

    return tree;
}

primary_index_type build_primary_index(const Relation &relation, const ColumnStore &store)
{
    primary_index_type primary_index;

    auto &orderkey_col = store.get_column<uint32_t>(relation["orderkey"].offset());
    auto &linenumber_col = store.get_column<uint32_t>(relation["linenumber"].offset());
    auto &comment_col = store.get_column<Char<45>>(relation["comment"].offset());

    auto linenumber_it = linenumber_col.begin();
    auto comment_it = comment_col.begin();

    asm volatile ("" : : : "memory");
    const auto mem_before = get_memory_reserved();
    const auto time_begin = high_resolution_clock::now();

    for (auto orderkey : orderkey_col) {
        auto res = primary_index.insert({{orderkey, *linenumber_it++}, &*comment_it++});
        (void) res; assert(res.second);
    }

    asm volatile ("" : : : "memory");
    const auto time_end = high_resolution_clock::now();
    const auto mem_after = get_memory_reserved();

    std::cout << "Milestone3, Q4, insert, "
              << primary_index.size() << ", "
              << duration_cast<nanoseconds>(time_end - time_begin).count() / 1e6 << " ms, "
              << (mem_after - mem_before) / 1024.f << " MiB\n";

    return primary_index;
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

    Relation orders("orders", {
            /* 00 */ Attribute::Int8("totalprice"),
            /* 01 */ Attribute::Int4("orderkey"),
            /* 02 */ Attribute::Int4("custkey"),
            /* 03 */ Attribute::Int4("orderdate"),
            /* 04 */ Attribute::Int4("shippriority"),
            /* 05 */ Attribute::Char("orderpriority", 16),
            /* 06 */ Attribute::Char("clerk", 16),
            /* 07 */ Attribute::Char("comment", 80),
            /* 08 */ Attribute::Int1("orderstatus"),
            });

    if (argc != 6)
        errx(EXIT_FAILURE, "Usage: %s <NUM_ROWS> <LINEITEM.tbl> <ORDERS.tbl> <ORDERKEY> <LINENUMBER>", argv[0]);
    std::size_t num_rows = atoll(argv[1]);
    const char *f_lineitem = argv[2];
    const char *f_orders = argv[3];
    const uint32_t O = atoi(argv[4]);
    const uint32_t L = atoi(argv[5]);

    /* Create the column stores. */
    ColumnStore *lineitem_store = new ColumnStore(ColumnStore::Create_Naive(lineitem));
    ColumnStore *orders_store = new ColumnStore(ColumnStore::Create_Naive(orders));

    /* Fill the stores. */
    num_rows = Loader::load_LineItem(f_lineitem, lineitem, *lineitem_store, num_rows);
    Loader::load_Orders(f_orders, orders, *orders_store, num_rows / 3);

    chunk *head = nullptr;

    /* Build the B+-Tree. */
    head = exhaust_reserved_memory(head);
    auto shipdate_index = build_shipdate_index(lineitem, *lineitem_store);

    /* Build the hash table. */
    head = exhaust_reserved_memory(head);
    auto primary_index = build_primary_index(lineitem, *lineitem_store);

    /* Compress the column store. */
    head = exhaust_reserved_memory(head);
    ColumnStore *compressed_columnstore = compress_columnstore_lineitem(lineitem, *lineitem_store);

#define BENCHMARK(QUERY, ...) { \
    const char *qstr = #QUERY; \
    head = exhaust_reserved_memory(head); \
    asm volatile ("" : : : "memory"); \
    const auto mem_before = get_memory_reserved(); \
    auto start = high_resolution_clock::now(); \
    auto result = query::milestone3:: QUERY(__VA_ARGS__); \
    asm volatile ("" : : : "memory"); \
    const auto mem_after = get_memory_reserved(); \
    auto stop = high_resolution_clock::now(); \
    std::cout << "Milestone3, " << qstr << ", compressed_columnstore, " << result << ", " \
              << duration_cast<nanoseconds>(stop - start).count() / 1e6 << " ms, " \
              << (mem_after - mem_before) / 1024.f << " MiB" \
              << std::endl; \
}

    /* Execute the queries. */
    BENCHMARK(Q3, *compressed_columnstore, shipdate_index);
    BENCHMARK(Q4, *compressed_columnstore, O, L, primary_index);
    BENCHMARK(Q5, *compressed_columnstore, *orders_store);

    delete lineitem_store;
    delete orders_store;
    delete compressed_columnstore;

    while (head) {
        chunk *c = head;
        head = c->next;
        delete c;
    }
}
