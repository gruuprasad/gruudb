#include "catch.hpp"
#include "dbms/Schema.hpp"
#include "impl/RowStore.hpp"
#include <cstdint>
#include <string>
#include <iostream>

using namespace dbms;

namespace {

Relation relation("relation", {
        Attribute::Int1("int1"),                // 1 bytes + 3 padding
        Attribute::Float("float"),              // 4 bytes
        Attribute::Int8("int8"),                // 8 bytes
        Attribute::Char("Char(3)", 3),          // 3 bytes + 5 padding
        Attribute::Double("double"),            // 8 bytes
        Attribute::Varchar("Varchar(42)", 42),  // 8 bytes
        });                                     // TOTAL: 40 bytes


Relation lineitem("lineitem", {
            Attribute::Int1("returnflag"),            // 1 bytes + 7 padding
            Attribute::Int8("extendedprice"),         // 8 bytes 
            Attribute::Int1("linestatus"),            // 1 bytes + 7 padding
            Attribute::Int8("tax"),                   // 8 bytes 
            Attribute::Int4("orderkey"),              // 4 bytes
            Attribute::Int8("discount"),              // 8 bytes
            Attribute::Char("shipinstruct", 26),      // 26 bytes + 2 padding
            Attribute::Int4("partkey"),               // 4 bytes
            Attribute::Int4("suppkey"),               // 4 bytes
            Attribute::Int4("commitdate"),            // 4 bytes
            Attribute::Int4("receiptdate"),           // 4 bytes
            Attribute::Int4("shipdate"),              // 4 bytes
            Attribute::Int4("linenumber"),            // 4 bytes
            Attribute::Char("shipmode", 11),          // 11 bytes
            Attribute::Char("comment", 45),           // 45 bytes
            Attribute::Int8("quantity"),              // 8 bytes
            });
}

TEST_CASE("RowStore/Naive", "[unit]")
{
    RowStore store = RowStore::Create_Naive(lineitem);

    REQUIRE(store.num_attributes() == 16);
    REQUIRE(store.row_size() == 168);
    REQUIRE(store.size() == 0);
    REQUIRE(store.size_in_bytes() == 0);

    SECTION("reserving memory increases capacity but not change size") {
        store.reserve(10);
        REQUIRE(store.size() == 0);
        REQUIRE(store.capacity() >= 10);
    }
    
    SECTION("reserving smaller does not change size or capacity") {
        store.reserve(25);
        REQUIRE(store.size() == 0);
        REQUIRE(store.capacity() >= 25);
        store.reserve(10);
        REQUIRE(store.size() == 0);
        REQUIRE(store.capacity() >= 25);
    }

    SECTION("insertion of first element triggers memory allocation") {
        store.append(1);
        CHECK(store.size() == 1);
        CHECK(store.capacity() >= ROW_CHUNK);
    }
    
    SECTION("the capacity is not smaller than the size") {
        for (std::size_t i = 0; i != 42; ++i)
            store.append(1);
        CHECK(store.size() == 42);
        CHECK(store.capacity() >= 42);
    }
    
    SECTION("insert smaller change size but not capacity") {
        store.append(42);
        REQUIRE(store.size() == 42);
        const auto old_capacity = store.capacity();
        REQUIRE(store.capacity() >= 42);
        store.append(137);
        CHECK(store.size() == 179);
        CHECK(store.capacity() == old_capacity);
    }

    SECTION("insert bigger change size and capacity") {
        store.append(25000);
        CHECK(store.size() == 25000);
        CHECK(store.capacity() == 25000);
        store.append(1);
        CHECK(store.size() == 25001);
        CHECK(store.capacity() == 25000 + ROW_CHUNK);
    }

    //TODO Write neat test case
    /* SECTION("rows can be read and written through the iterator interface") { */
    /*     REQUIRE(store.size() == 0); */

    /*     { */
    /*         auto it = store.append(5); */
    /*         REQUIRE(store.capacity() >= 5); */
    /*         for (std::size_t i = 0; i != 5; ++i, ++it) { */
    /*             it.get<uint32_t>(0) =  i; */
    /*             it.get<uint32_t>(1) = (1lu << 20) + i; */
    /*             it.get<uint32_t>(2) = (1lu << 20) + i; */
    /*             it.get< int32_t>(3) = i; */
    /*             it.get< int64_t>(4) = i; */
    /*             it.get< int64_t>(5) = i; */
    /*             it.get< int64_t>(6) = i; */
    /*             it.get< int64_t>(7) = i; */
    /*             it.get<unsigned char>(8) = i; */
    /*             it.get<unsigned char>(9) = i; */
    /*             it.get<uint32_t>(10) = (1lu << 42) + i; */
    /*             it.get<uint32_t>(11) = (1lu << 42) + i; */
    /*             it.get<uint32_t>(12) = (1lu << 42) + i; */
    /*             it.get<Char<26>>(13) = "ship"; */
    /*             it.get<Char<11>>(14) = "plane"; */
    /*             it.get<Char<45>>(15) = "nocomment"; */
    /*         } */
    /*     } */

    /*     { */
    /*         auto it = store.begin(); */
    /*         for (std::size_t i = 0; i != 5; ++i, ++it) { */
    /*             CHECK(it.get<uint32_t>(0) == (1lu << 20) + i); */
    /*             CHECK(it.get<uint32_t>(1) == (1lu << 20) + i); */
    /*             CHECK(it.get<uint32_t>(2) == (1lu << 20) + i); */
    /*             CHECK(it.get< int32_t>(3) == (1lu << 20) + i); */
    /*             CHECK(it.get< int64_t>(4) == (1lu << 42) + i); */
    /*             CHECK(it.get< int64_t>(5) == (1lu << 42) + i); */
    /*             CHECK(it.get< int64_t>(6) == (1lu << 42) + i); */
    /*             CHECK(it.get< int64_t>(7) == (1lu << 42) + i); */
    /*             CHECK(it.get<unsigned char>(8) == "OK"); */
    /*             CHECK(it.get<unsigned char>(9) == "OK"); */
    /*             CHECK(it.get<uint32_t>(10) == (1lu << 20) + i); */
    /*             CHECK(it.get<uint32_t>(11) == (1lu << 20) + i); */
    /*             CHECK(it.get<uint32_t>(12) == (1lu << 20) + i); */
    /*             CHECK(it.get<Char<26>>(13) == "ship"); */
    /*             CHECK(it.get<Char<11>>(14) == "plane"); */
    /*             CHECK(it.get<Char<45>>(15) == "nocomment"); */           
    /*         } */
    /*     } */
    /* } */
}
