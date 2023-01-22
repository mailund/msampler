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
    if (!Range::in_range(val))
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

struct Open {};
struct Closed {};

template <typename Bounds, typename L, typename U> class Range {
  // Dispatching on boundaries Open/CLose for checking edges
  template <typename> static bool above(double val);
  template <typename> static bool below(double val);
  template <> static constexpr bool above<Open>(double val) {
    return Bounds::lower_bound < val;
  }
  template <> static constexpr bool above<Closed>(double val) {
    return Bounds::lower_bound <= val;
  }
  template <> static constexpr bool below<Open>(double val) {
    return val < Bounds::upper_bound;
  }
  template <> static bool below<Closed>(double val) {
    return val <= Bounds::upper_bound;
  }

public:
  static bool above_lower(double v) { return above<L>(v); }
  static bool below_upper(double v) { return below<U>(v); }
  static bool in_range(double v) { return above_lower(v) && below_upper(v); }
};

struct NonNegativeBounds : Range<NonNegativeBounds, Closed, Open> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = std::numeric_limits<double>::infinity();
};
using NonNegative = Bounded<NonNegativeBounds>;

struct UnitBounds : Range<UnitBounds, Closed, Closed> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = 1.0;
};
using Unit = Bounded<UnitBounds>;

} // namespace bounded
