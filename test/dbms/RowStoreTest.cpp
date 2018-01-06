#include "catch.hpp"
#include "dbms/Schema.hpp"
#include "impl/RowStore.hpp"
#include <cstdint>
#include <string>


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

}

TEST_CASE("RowStore/Naive", "[unit][milestone1]")
{
    RowStore store = RowStore::Create_Naive(relation);

    REQUIRE(store.num_attributes() == 6);
    REQUIRE(store.row_size() == 40);
    REQUIRE(store.size() == 0);
    REQUIRE(store.size_in_bytes() == 0);

    SECTION("insertion of the first element triggers memory allocation") {
        store.append(1);
        CHECK(store.size() == 1);
        CHECK(store.capacity() >= 1);
        CHECK(store.capacity_in_bytes() >= 40);
    }

    SECTION("the capacity is not smaller than the size") {
        for (std::size_t i = 0; i != 42; ++i)
            store.append(1);
        CHECK(store.size() == 42);
        CHECK(store.capacity() >= 42);
    }

    SECTION("reserving bigger increases capacity but does not change size") {
        store.append(42);
        REQUIRE(store.size() == 42);
        REQUIRE(store.capacity() >= 42);
        store.reserve(137);
        CHECK(store.size() == 42);
        CHECK(store.capacity() >= 137);
    }

    SECTION("reserving smaller does not change size or capacity") {
        store.append(13);
        store.reserve(42);
        REQUIRE(store.size() == 13);
        REQUIRE(store.capacity() >= 42);
        const auto old_cap = store.capacity();
        store.reserve(5);
        REQUIRE(store.size() == 13);
        CHECK(store.capacity() == old_cap);
    }

    SECTION("rows can be read and written through the iterator interface") {
        REQUIRE(store.size() == 0);

        {
            auto it = store.append(5);
            REQUIRE(store.capacity() >= 5);
            for (std::size_t i = 0; i != 5; ++i, ++it) {
                it.get<uint8_t>(0) = i;
                it.get<float>(1) = 3.14f * i;
                it.get<int64_t>(2) = (1lu << 42) + i;
                it.get<Char<3>>(3) = "OK";
                it.get<double>(4) = 2.71828 * i;
                it.get<const char*>(5) = strdup(std::to_string(i).c_str());
            }
        }

        {
            auto it = store.begin();
            for (std::size_t i = 0; i != 5; ++i, ++it) {
                CHECK(it.get<uint8_t>(0) == i);
                CHECK(it.get<float>(1) == 3.14f * i);
                CHECK(it.get<int64_t>(2) == (1lu << 42) + i);
                CHECK(std::string(it.get<Char<3>>(3)) == "OK");
                CHECK(it.get<double>(4) == 2.71828 * i);
                CHECK(std::to_string(i) == it.get<const char*>(5));
            }
        }
    }
}

TEST_CASE("RowStore/Optimized", "[unit][milestone1]")
{
    RowStore store = RowStore::Create_Optimized(relation);

    REQUIRE(store.num_attributes() == 6);
    REQUIRE(store.size() == 0);
    REQUIRE(store.size_in_bytes() == 0);

    SECTION("insertion of the first element triggers memory allocation") {
        store.append(1);
        CHECK(store.size() == 1);
        CHECK(store.capacity() >= 1);
    }

    SECTION("rows can be read and written through the iterator interface") {
        REQUIRE(store.size() == 0);

        {
            auto it = store.append(5);
            REQUIRE(store.capacity() >= 5);
            for (std::size_t i = 0; i != 5; ++i, ++it) {
                it.get<uint8_t>(0) = i;
                it.get<float>(1) = 3.14f * i;
                it.get<int64_t>(2) = (1lu << 42) + i;
                it.get<Char<3>>(3) = "OK";
                it.get<double>(4) = 2.71828 * i;
                it.get<const char*>(5) = strdup(std::to_string(i).c_str());
            }
        }

        {
            auto it = store.begin();
            for (std::size_t i = 0; i != 5; ++i, ++it) {
                CHECK(it.get<uint8_t>(0) == i);
                CHECK(it.get<float>(1) == 3.14f * i);
                CHECK(it.get<int64_t>(2) == (1lu << 42) + i);
                CHECK(std::string(it.get<Char<3>>(3)) == "OK");
                CHECK(it.get<double>(4) == 2.71828 * i);
                CHECK(std::to_string(i) == it.get<const char*>(5));
            }
        }
    }
}
