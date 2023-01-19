#pragma once

#include <vector>

/// Multinomial sampler class.
/// Initialisation takes linear time, but after that you can sample
/// in constant time.
class MultinomialSampler {
public:
  using dist = std::vector<double>;

  /// @brief Initialise the multinomial sampler with outcome probabilities.
  /// @param weights The initial probabilities. Must be non-empty and weights
  /// must be positive, but need not sum to one.
  MultinomialSampler(dist weights);

  /// @brief  The probability we have associated with outcome i
  /// @param i An outcome index
  /// @return The initial probability assigned to i.
  double outcome_probability(int i) const { return initial_probs_[i]; }

  /// @brief Calculate the probability we have for sampling i.
  ///
  /// This should be the same probability as `outcome_probability(i)`, but
  /// calculated the way we sample (so there can be some small numerical
  /// differences).
  ///
  /// @param i An outcome index
  /// @return The probability of sampling i.
  double sample_probability(int i) const;

  int sample() const {
    int bucket = un();
    double u = u01();
    return (u < accept_probs_[bucket]) ? first_choice_[bucket]
                                       : second_choice_[bucket];
  }

private:
  /// @brief Holds the probabilities the sampler was initialised with.
  /// Unlike the weights we initialise with, this one will be normalised to
  /// zero.
  dist initial_probs_;
  /// @brief The probability that we accept an outcome after the first sample.
  dist accept_probs_;
  /// @brief The outcome we get if we accept the first choice.
  std::vector<int> first_choice_;
  /// @brief The alternative outcome we take if we don't accept the first.
  std::vector<int> second_choice_;

  /// @brief  Sample uniformly on [0,1]
  /// @return x where x ~ U(0,1)
  double u01() const;

  /// @brief Sample uniformly in [0, 1, ..., n-1] where n is the size of the
  /// distribution
  /// @return Uniformly distributed integer between zero and n (n not included).
  int un() const;
};
