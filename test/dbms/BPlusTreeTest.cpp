#include "catch.hpp"
#include "dbms/util.hpp"
#include "impl/BPlusTree.hpp"


using namespace dbms;


TEST_CASE("BPlusTree", "[unit][milestone3]")
{
    using BPTree = BPlusTree<int, int>;
    constexpr std::size_t NUM_ELEMENTS = 137;

    /* Create test data. */
    std::vector<std::pair<int, int>> data;
    for (std::size_t i = 0, step = 0; i < NUM_ELEMENTS; ++step, i += step)
        data.push_back({int(i), int(step)});

    auto tree = BPTree::Bulkload(data.begin(), data.end());

    /* Test point queries. */
    {
        auto it = tree.find(15);
        REQUIRE(it != tree.end());
        REQUIRE(it->first == 15);
        REQUIRE(it->second == 5);
    }

    {
        auto it = tree.find(16);
        REQUIRE(it == tree.end());
    }

    {
        auto it = tree.find(0);
        REQUIRE(it == tree.begin());
        REQUIRE(it->first == 0);
        REQUIRE(it->second == 0);
    }

    {
        auto it = tree.find(136);
        REQUIRE(it != tree.end());
        REQUIRE(it->first == 136);
        REQUIRE(it->second == 16);
        ++it;
        REQUIRE(it == tree.end());
    }

    {
        auto it = tree.find(142);
        REQUIRE(it == tree.end());
    }

    /* Test range queries. */
    {
        auto range = tree.in_range(13, 42);
        auto it = range.begin();
        REQUIRE(it->first == 15);
        ++it;
        REQUIRE(it->first == 21);
        ++it;
        REQUIRE(it->first == 28);
        ++it;
        REQUIRE(it->first == 36);
        ++it;
        REQUIRE(it == range.end());
    }
}
