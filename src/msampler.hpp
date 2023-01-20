#pragma once

#include <limits>
#include <vector>

namespace bounded {

/// @brief Bound a value of type T to be within a range
///
/// Although C++20 does allow floats as non-type template arguments, clang
/// doesn't implement it yet. So, instead of having bounds of type T, we have
/// to use references to T, so we need to create symbols for the bounds.
///
/// @tparam T Type (of some numeric type)
/// @tparam min_bound the smallest value allowed
/// @tparam max_bound the largest value allowed
template <typename T, const T &min_bound, const T &max_bound> class Bounded {
  T val;

public:
  Bounded(T v) : val{v} {
    if (val < min_bound || max_bound < val)
      throw std::out_of_range(
          "Numerical value is outside of the permitted range");
  }
  Bounded() = default;
  Bounded(Bounded &other) = default;
  Bounded(Bounded &&other) = default;
  Bounded &operator=(Bounded &other) = default;
  Bounded &operator=(Bounded &&other) = default;

  operator T() const { return val; }
  T operator+(const Bounded &other) const { return val + other.val; }
  T operator-(const Bounded &other) const { return val - other.val; }
  T operator*(const Bounded &other) const { return val * other.val; }
  T operator/(const Bounded &other) const { return val / other.val; }
};

const auto zero = 0.0;
const auto one = 1.0;
const auto inf = std::numeric_limits<double>::max();
using U01 = Bounded<double, zero, one>;

using Weight = Bounded<double, zero, inf>;
using Weights = std::vector<Weight>;

} // namespace bounded

/// @brief Multinomial sampler class.
///
/// Initialisation takes linear time, but after that you can sample
/// in constant time.
///
class MultinomialSampler {
public:
  using Weights = std::vector<double>;
  using Probabilities = std::vector<double>;
  using Outcome = unsigned int;

  /// @brief Initialise the multinomial sampler with outcome probabilities.
  /// @param w The initial probabilities. Must be non-empty and weights
  /// must be positive, but need not sum to one.
  MultinomialSampler(Weights w);

  /// @brief Get the number of possible outcomes.
  /// @return The size of the multinomial distribution.
  size_t no_outcomes() const { return initial_probs_.size(); }

  /// @brief  The probability we have associated with outcome i
  /// @param i An outcome index
  /// @return The initial probability assigned to i.
  double outcome_probability(Outcome i) const { return initial_probs_[i]; }

  /// @brief Calculate the probability we have for sampling i.
  ///
  /// This should be the same probability as `outcome_probability(i)`, but
  /// calculated the way we sample (so there can be some small numerical
  /// differences).
  ///
  /// @param i An outcome index
  /// @return The probability of sampling i.
  double sample_probability(int i) const;

  Outcome sample() const {
    int bucket = un();
    double u = u01();
    return (u < accept_probs_[bucket]) ? first_choice_[bucket]
                                       : second_choice_[bucket];
  }

private:
  /// @brief Holds the probabilities the sampler was initialised with.
  /// Unlike the weights we initialise with, this one will be normalised to
  /// zero.
  Probabilities initial_probs_;
  /// @brief The probability that we accept an outcome after the first sample.
  Probabilities accept_probs_;
  /// @brief The outcome we get if we accept the first choice.
  std::vector<Outcome> first_choice_;
  /// @brief The alternative outcome we take if we don't accept the first.
  std::vector<Outcome> second_choice_;

  /// @brief  Sample uniformly on [0,1]
  /// @return x where x ~ U(0,1)
  double u01() const;

  /// @brief Sample uniformly in [0, 1, ..., n-1] where n is the size of the
  /// distribution
  /// @return Uniformly distributed integer between zero and n (n not included).
  int un() const;
};
