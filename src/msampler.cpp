#include "msampler.hpp"

#include <numeric>
#include <ranges>

// Need to clean up this function!
MultinomialSampler::MultinomialSampler(
    std::initializer_list<bounded::NonNegative> w)
    : initial_probs_(w.size()), accept_probs_(w.size()),
      first_choice_(w.size()), second_choice_(w.size()) {

  if (w.size() == 0)
    throw std::invalid_argument("Empty weights");

  double sum = std::reduce(w.begin(), w.end());
  if (sum == 0.0)
    throw std::invalid_argument("Weights sum to zero");

  // Normalise the initial probabilities
  for (int i = 0; auto p : w) {
    initial_probs_[i++] = p / sum;
  }

  // accept_probs_ holds [0,1] but during the calculation we need
  // values greater than one, so we use a double vector here.
  int n = initial_probs_.size();
  std::vector<double> acc(n);

  // Order probabilities with the small on the left
  // and the large on the right.
  int l = 0, r = n;
  for (int i = 0; i < n; ++i) {
    double weight = initial_probs_[i] * n;
    if (weight < 1.0) {
      acc[l] = weight;
      first_choice_[l] = i;
      ++l;
    } else {
      --r;
      acc[r] = weight;
      first_choice_[r] = i;
    }
  }
  // Now the neds should have met and l should
  // point at the first large (if there are any large).
  assert(l == r);
  assert(l < n ? acc[l] >= 1 : true);

  // Now move probability from the large to the small in the
  // second choices
  // ensure that an uninitialised second choice is not confused with
  // choosing 0.
  std::fill(second_choice_.begin(), second_choice_.end(), -1);
  for (int i = 0; i < l; ++i) {
    // small will pick the large as second choice
    second_choice_[i] = first_choice_[l];
    acc[l] -= (1.0 - acc[i]);
    if (acc[l] < 1.0)
      ++l; // l is no longer large, so we move to the next large
  }

  // Sanity check... some outcomes won't have a second, and they should
  // have an acceptance probability of 1.0. If they don't, we could
  // sample an invalid second choice if we reject the first.
  for (int i = 0; i < n; ++i) {
    if (second_choice_[i] == -1) {
      assert(acc[i] == 1.0);
    }
  }

  // Now the acceptance probabilities should be in the range [0,1]
  // so we can copy them to accept_probs_
  std::copy(acc.begin(), acc.end(), accept_probs_.begin());
};

bounded::Unit MultinomialSampler::sample_probability(int i) const {
  // Cannot be a probability yet, but will be when we normalise at the end
  double weight = 0.0;
  const int n = accept_probs_.size();

  for (int j = 0; j < n; j++) {
    weight += (first_choice_[j] == i) * (accept_probs_[j]) +
              (second_choice_[j] == i) * (1.0 - accept_probs_[j]);
  }

  return weight / accept_probs_.size();
}
