#include "catch.hpp"
#include "dbms/util.hpp"
#include "impl/HashTable.hpp"


using namespace dbms;


TEST_CASE("HashTable", "[unit][milestone3]")
{
    const char *str0 = "Hello, World";
    const char *str1 = "This is a test";
    const char *str2 = "This string is new";

    using table_t = HashTable<const char*, StrHash, StrEqual>;
    table_t table;

    REQUIRE(table.size() == 0);

#define INSERT(V, R) {\
    auto res = table.insert(V); \
    REQUIRE((R) == res.second); \
    REQUIRE(std::string(V) == *res.first); \
}
#define INSERT_SUCCESS(V) INSERT(V, true)
#define INSERT_FAIL(V) INSERT(V, false)

    INSERT_SUCCESS(str0);
    INSERT_SUCCESS(str1);
    INSERT_FAIL(str0);
    INSERT_SUCCESS(str2);
    INSERT_FAIL(str1);
    INSERT_FAIL(str2);
}

TEST_CASE("HashTable/grow", "[unit][milestone3]")
{
    using table_t = HashTable<int>;
    table_t table;
    const table_t::size_type initial_capacity = table.capacity();
    const table_t::size_type num_insert = initial_capacity + 1;

    for (table_t::size_type i = 1; i <= num_insert; ++i)
        table.insert(i);

    REQUIRE(table.capacity() >= num_insert);
    REQUIRE(table.size() == num_insert);

    /* Check no elements were lost during resize. */
    for (table_t::size_type i = 1; i <= num_insert; ++i)
        REQUIRE(not table.insert(i).second);

    std::size_t count = 0;
    for (auto elem : table)
        ++count;
    REQUIRE(count == num_insert);
}

TEST_CASE("hash_set", "[unit][milestone3]")
{
    hash_set<int> set;

    for (int i = 1; i < 42; i *= -2) {
        const auto res = set.insert(i);
        REQUIRE(res.second);
        REQUIRE(*res.first == i);
    }

    for (int i = 1; i < 42; i *= -2) {
        const auto it = set.find(i);
        REQUIRE(it != set.end());
        REQUIRE(*it == i);
    }

    for (int i = 1; i < 42; i *= -2) {
        const auto res = set.insert(i);
        REQUIRE(not res.second);
        REQUIRE(*res.first == i);
    }
}

TEST_CASE("hash_map/insert", "[unit][milestone3]")
{
    hash_map<int, int> map;

    for (int i = 1; i < 42; i *= -2) {
        const auto res = map.insert({i, 2*i});
        REQUIRE(res.second);
        REQUIRE(res.first->first == i);
        REQUIRE(res.first->second == 2*i);
    }

    for (int i = 1; i < 42; i *= -2) {
        const auto it = map.find(i);
        REQUIRE(it != map.end());
        REQUIRE(it->first == i);
        REQUIRE(it->second == 2*i);
    }
}

TEST_CASE("hash_map/operator[]", "[unit][milestone3]")
{
    hash_map<int, int> map;

    for (int i = 1; i < 42; i *= -2)
        map[i] = 2 * i;

    for (int i = 1; i < 42; i *= -2) {
        const auto it = map.find(i);
        REQUIRE(it != map.end());
        REQUIRE(it->first == i);
        REQUIRE(it->second == 2*i);
    }
}
