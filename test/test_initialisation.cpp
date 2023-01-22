#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <msampler.hpp>

TEST_CASE("Valid initialsation normalises weights", "[init]") {
  SECTION("uniform dist") {
    MultinomialSampler sampler{1.0 / 3, 1.0 / 3, 1.0 / 3};
    for (int i = 0; i < sampler.no_outcomes(); i++) {
      auto expected = Catch::Approx(1.0 / 3);
      double got = sampler.outcome_probability(i);
      REQUIRE(got == expected.margin(0.0001).epsilon(1e-12));
    }
  }

  SECTION("skewed dist") {
    MultinomialSampler sampler{1, 2, 3};
    for (int i = 0; i < sampler.no_outcomes(); i++) {
      auto expected = Catch::Approx(static_cast<double>(i + 1) / 6);
      double got = sampler.outcome_probability(i);
      REQUIRE(got == expected.margin(0.0001).epsilon(1e-12));
    }
  }

  SECTION("skewed dist reverse") {
    MultinomialSampler sampler{3, 2, 1};
    for (int i = 0; i < sampler.no_outcomes(); i++) {
      auto expected =
          Catch::Approx(static_cast<double>(sampler.no_outcomes() - i) / 6);
      double got = sampler.outcome_probability(i);
      REQUIRE(got == expected.margin(0.0001).epsilon(1e-12));
    }
  }
}

TEST_CASE("Initial probs matches sampling probs", "[init]") {
  SECTION("uniform dist") {
    MultinomialSampler sampler{1.0 / 3, 1.0 / 3, 1.0 / 3};
    for (int i = 0; i < sampler.no_outcomes(); i++) {
      auto init = Catch::Approx(sampler.outcome_probability(i));
      double sample = sampler.sample_probability(i);
      REQUIRE(sample == init.margin(0.0001).epsilon(1e-12));
    }
  }

  SECTION("skewed dist") {
    MultinomialSampler sampler{1, 2, 3};
    for (int i = 0; i < sampler.no_outcomes(); i++) {
      auto init = Catch::Approx(sampler.outcome_probability(i));
      double sample = sampler.sample_probability(i);
      REQUIRE(sample == init.margin(0.0001).epsilon(1e-12));
    }
  }
}

TEST_CASE("Invalid initialsation", "[init]") {
  SECTION("empty outcomes") {
    REQUIRE_THROWS_AS(MultinomialSampler{}, std::invalid_argument);
  }
  SECTION("weight zero") {
    REQUIRE_THROWS_AS(MultinomialSampler({0, 0, 0}), std::invalid_argument);
  }
}

TEST_CASE("Sampling", "[sample]") {
  SECTION("Test with uniform distribution") {

    MultinomialSampler sampler{1.0 / 3, 1.0 / 3, 1.0 / 3};

    // Test that if we pick the first bucket always, we get
    // a uniform sample
    {
      int samples[] = {0, 0, 0};
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        bounded::Unit u0 = i / 3.0;
        bounded::Unit u1 = 0.0;
        samples[sampler.sample(u0, u1)]++;
      }
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        REQUIRE(samples[i] == 1);
      }
    }
    {
      int samples[] = {0, 0, 0};
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        bounded::Unit u0 = i / 3.0;
        bounded::Unit u1 = 0.1;
        samples[sampler.sample(u0, u1)]++;
      }
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        REQUIRE(samples[i] == 1);
      }
    }
    {
      int samples[] = {0, 0, 0};
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        bounded::Unit u0 = i / 3.0;
        bounded::Unit u1 = 0.9999;
        samples[sampler.sample(u0, u1)]++;
      }
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        REQUIRE(samples[i] == 1);
      }
    }
    {
      int samples[] = {0, 0, 0};
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        bounded::Unit u0 = i / 3.0;
        bounded::Unit u1 = 1.0;
        samples[sampler.sample(u0, u1)]++;
      }
      for (int i = 0; i < sampler.no_outcomes(); i++) {
        REQUIRE(samples[i] == 1);
      }
    }
  }
}