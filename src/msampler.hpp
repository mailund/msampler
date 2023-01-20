#pragma once

#include <vector>

#include "bounds.hpp"

/// @brief Multinomial sampler class.
///
/// Initialisation takes linear time, but after that you can sample
/// in constant time.
///
class MultinomialSampler {
public:
  using Outcome = unsigned int;

  /// @brief Initialise the multinomial sampler with outcome probabilities.
  /// @param weights The initial probabilities. Must be non-empty and weights
  /// must be positive, but need not sum to one.
  MultinomialSampler(std::initializer_list<bounded::NonNegative> weights);

  /// @brief Get the number of possible outcomes.
  /// @return The size of the multinomial distribution.
  size_t no_outcomes() const { return initial_probs_.size(); }

  /// @brief  The probability we have associated with outcome i
  /// @param i An outcome index
  /// @return The initial probability assigned to i.
  bounded::Unit outcome_probability(Outcome i) const {
    return initial_probs_[i];
  }

  /// @brief Calculate the probability we have for sampling i.
  ///
  /// This should be the same probability as `outcome_probability(i)`, but
  /// calculated the way we sample (so there can be some small numerical
  /// differences).
  ///
  /// @param i An outcome index
  /// @return The probability of sampling i.
  bounded::Unit sample_probability(int i) const;

  /// @brief Get an outcome from two uniformly (U[0,1]) distributed variables.
  /// @param u0 ~ U[0,1] ; picks the initial sample
  /// @param u1 ~ U[0,1] ; accepts the first sample, or picks the next
  /// @return An outcome that, if u0 and u1 are randomly chosen, is sampled from
  ///         the multinomail distribution.
  Outcome sample(bounded::Unit u0, bounded::Unit u1) const {
    // With zero probability we pick u0 == 1.0, but we can't have]
    // no_outcomes() -- it is one too high -- so we put that in the
    // last bucket.
    int bucket = (u0 < 1.0) ? u0 * no_outcomes() : no_outcomes() - 1;
    return (u1 <= accept_probs_[bucket]) ? first_choice_[bucket]
                                         : second_choice_[bucket];
  }

private:
  /// @brief Holds the probabilities the sampler was initialised with.
  /// Unlike the weights we initialise with, this one will be normalised to
  /// zero.
  std::vector<bounded::Unit> initial_probs_;
  /// @brief The probability that we accept an outcome after the first sample.
  std::vector<bounded::Unit> accept_probs_;
  /// @brief The outcome we get if we accept the first choice.
  std::vector<Outcome> first_choice_;
  /// @brief The alternative outcome we take if we don't accept the first.
  std::vector<Outcome> second_choice_;
};
