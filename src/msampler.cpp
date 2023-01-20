#include "msampler.hpp"

// Need to clean up this function!
MultinomialSampler::MultinomialSampler(Weights w) {
  if (w.size() == 0)
    throw std::invalid_argument("Empty weights");

  // FIXME: clean this up
  for (double p : w) {
    initial_probs_.push_back(p);
  }

  // Check if the weights are valid
  double sum = 0.0;
  for (double p : initial_probs_) {
    if (p <= 0.0)
      throw std::invalid_argument("Non-positive weight");
    sum += p;
  }

  // Then normalise the initial probabilities
  for (double &p : initial_probs_) {
    p /= sum;
  }

  int n = initial_probs_.size();
  accept_probs_.resize(n);
  first_choice_.resize(n);
  second_choice_.resize(n);
  std::fill(second_choice_.begin(), second_choice_.end(), -1);

  // Order probabilities with the small on the left
  // and the large on the right.
  int l = 0, r = n;
  for (int i = 0; i < n; ++i) {
    double weight = initial_probs_[i] * n;
    if (weight < 1.0) {
      accept_probs_[l] = weight;
      first_choice_[l] = i;
      ++l;
    } else {
      --r;
      accept_probs_[r] = weight;
      first_choice_[r] = i;
    }
  }
  // Now the neds should have met and l should
  // point at the first large (if there are any large).
  assert(l == r);
  assert(l < n ? accept_probs_[l] >= 1 : true);

  // Now move probability from the large to the small in the
  // second choices
  for (int i = 0; i < l; ++i) {
    second_choice_[i] =
        first_choice_[l]; // small will pick the large as second choice
    accept_probs_[l] -=
        (1.0 - accept_probs_[i]); // now we have moved this prob.
    if (accept_probs_[l] < 1.0)
      ++l; // l is no longer large, so we move to the next large
  }
};

double MultinomialSampler::sample_probability(int i) const {
  double weight = 0.0;
  const int n = accept_probs_.size();

  for (int j = 0; j < n; j++) {
    if (first_choice_[j] == i)
      weight += accept_probs_[j];
    if (second_choice_[j] == i)
      weight += 1.0 - accept_probs_[j];
  }
  return weight / accept_probs_.size();
}

// FIXME: the random number generators are just hacks. They are not
// good enough for a real sampling
double MultinomialSampler::u01() const {
  return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

int MultinomialSampler::un() const {
  // FIXME: This is *really* skewed if dist is small!
  return rand() % accept_probs_.size();
}
