#pragma once

#include <limits>

namespace bounded {

/// @brief Bound floating point numbers to a range
///
/// The use of Range is a hack to get around that we can't use floats
/// as template parameters. (C++20 does permit it, but clang doesn't
/// implement it yet).
///
/// @tparam Range must specify min_bound and max_bound
template <typename Range> class Bounded {
  /// @brief Wrapped value
  double val;

  /// @brief Throw an exception if we get a value outside the permitted range
  void validate() const {
    if (val < Range::min_bound || Range::max_bound < val)
      throw std::out_of_range(
          "Numerical value is outside of the permitted range");
  }

public:
  Bounded(double v) : val{v} { validate(); }
  Bounded() = default;
  Bounded(Bounded &other) = default;
  Bounded(Bounded &&other) = default;
  Bounded &operator=(Bounded &other) = default;
  Bounded &operator=(Bounded &&other) = default;

  operator double() const { return val; }
  template <typename R> double operator+(const Bounded<R> &other) const {
    return val + other.val;
  }
  template <typename R> Bounded &operator+=(const Bounded<R> &other) {
    val += other.val;
    validate();
    return *this;
  }
  Bounded &operator+=(double other) {
    val += other;
    validate();
    return *this;
  }

  template <typename R> double operator-(const Bounded &other) const {
    return val - other.val;
  }
  template <typename R> Bounded &operator-=(const Bounded<R> &other) {
    val -= other.val;
    validate();
    return *this;
  }
  Bounded &operator-=(double other) {
    val -= other;
    validate();
    return *this;
  }

  template <typename R> double operator*(const Bounded &other) const {
    return val * other.val;
  }
  template <typename R> Bounded &operator*=(const Bounded<R> &other) {
    val *= other.val;
    validate();
    return *this;
  }
  Bounded &operator*=(double other) {
    val *= other;
    validate();
    return *this;
  }

  template <typename R> double operator/(const Bounded &other) const {
    return val / other.val;
  }
  template <typename R> Bounded &operator/=(const Bounded<R> &other) {
    val /= other.val;
    validate();
    return *this;
  }
  Bounded &operator/=(double other) {
    val /= other;
    validate();
    return *this;
  }
};

struct NonNegativeBounds {
  static constexpr double min_bound = 0.0;
  static constexpr double max_bound = std::numeric_limits<double>::max();
};
using NonNegative = Bounded<NonNegativeBounds>;

struct UnitBounds {
  static constexpr double min_bound = 0.0;
  static constexpr double max_bound = 1.0;
};
using Unit = Bounded<UnitBounds>;

} // namespace bounded
