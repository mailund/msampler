#include <catch2/catch_test_macros.hpp>

#include <bounded.hpp>
using namespace range;

TEST_CASE("Constraining numbers", "[bounds]") {
  SECTION("Limited to [0,1]") {
    for (double x : {0.0, 0.5, 1.0}) {
      Unit y{x};
      REQUIRE(x == y);
    }
    REQUIRE_THROWS_AS(Unit{-0.1}, std::out_of_range);
    REQUIRE_THROWS_AS(Unit{1.1}, std::out_of_range);
  }
}