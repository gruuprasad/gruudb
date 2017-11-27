#include "catch.hpp"
#include "dbms/util.hpp"


TEST_CASE("ceil_to_pow2", "[unit][core]")
{
    REQUIRE(ceil_to_pow2(1) == 1);
    REQUIRE(ceil_to_pow2(2) == 2);
    REQUIRE(ceil_to_pow2(3) == 4);
    REQUIRE(ceil_to_pow2(4) == 4);
    REQUIRE(ceil_to_pow2(5) == 8);
    REQUIRE(ceil_to_pow2(7) == 8);
    REQUIRE(ceil_to_pow2(8) == 8);
}
