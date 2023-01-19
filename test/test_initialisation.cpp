#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <msampler.hpp>

TEST_CASE("Valid initialsation normalises weights", "[init]") {
  SECTION("uniform dist") {
    std::vector<double> p{1.0 / 3, 1.0 / 3, 1.0 / 3};
    MultinomialSampler sampler(p);
    for (int i = 0; i < 3; i++) {
      auto expected = Catch::Approx(1.0 / 3);
      double got = sampler.outcome_probability(i);
      REQUIRE(got == expected.margin(0.0001).epsilon(1e-12));
    }
  }

  SECTION("skewed dist") {
    std::vector<double> p{1, 2, 3};
    MultinomialSampler sampler(p);
    for (int i = 0; i < 3; i++) {
      auto expected = Catch::Approx(static_cast<double>(i + 1) / 6);
      double got = sampler.outcome_probability(i);
      REQUIRE(got == expected.margin(0.0001).epsilon(1e-12));
    }
  }
}

TEST_CASE("Initial probs matches sampling probs", "[init]") {
  SECTION("uniform dist") {
    std::vector<double> p{1.0 / 3, 1.0 / 3, 1.0 / 3};
    MultinomialSampler sampler(p);
    for (int i = 0; i < p.size(); i++) {
      auto init = Catch::Approx(sampler.outcome_probability(i));
      double sample = sampler.sample_probability(i);
      REQUIRE(sample == init.margin(0.0001).epsilon(1e-12));
    }
  }

  SECTION("skewed dist") {
    std::vector<double> p{1, 2, 3};
    MultinomialSampler sampler(p);
    for (int i = 0; i < p.size(); i++) {
      auto init = Catch::Approx(sampler.outcome_probability(i));
      double sample = sampler.sample_probability(i);
      REQUIRE(sample == init.margin(0.0001).epsilon(1e-12));
    }
  }
}

TEST_CASE("Invalid initialsation", "[init]") {
  SECTION("empty outcomes") {
    std::vector<double> p{};
    REQUIRE_THROWS_AS(MultinomialSampler(p), std::invalid_argument);
  }
  SECTION("negative weights") {
    std::vector<double> p{1, -2, 3};
    REQUIRE_THROWS_AS(MultinomialSampler(p), std::invalid_argument);
  }
  SECTION("weight zero") {
    std::vector<double> p{1, 0, 3};
    REQUIRE_THROWS_AS(MultinomialSampler(p), std::invalid_argument);
  }
}