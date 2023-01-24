#pragma once

#include <concepts>
#include <cstddef>
#include <limits>

#include "range.hpp"

namespace range {

/// @brief Bound floating point numbers to a range
///
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

  template <RangeType R>
  Bounded(Bounded<R> &other) : val(static_cast<double>(other)) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");
  }
  template <RangeType R>
  Bounded(Bounded<R> &&other) : val(static_cast<double>(other)) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");
  }

  template <RangeType R> Bounded &operator=(Bounded<R> &other) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");
    val = static_cast<double>(other);
    return *this;
  }
  template <RangeType R> Bounded &operator=(Bounded<R> &&other) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");
    val = static_cast<double>(other);
  }

  operator double() const { return val; }
  template <RangeType R> double operator+(const Bounded<R> &other) const {
    return val + static_cast<double>(other);
  }
  template <RangeType R> Bounded &operator+=(const Bounded<R> &other) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");

    val += static_cast<double>(other);
    validate();
    return *this;
  }
  Bounded &operator+=(double other) {
    val += static_cast<double>(other);
    validate();
    return *this;
  }

  template <RangeType R> double operator-(const Bounded<R> &other) const {
    return val - static_cast<double>(other);
  }
  template <RangeType R> Bounded &operator-=(const Bounded<R> &other) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");

    val -= static_cast<double>(other);
    validate();
    return *this;
  }
  Bounded &operator-=(double other) {
    val -= other;
    validate();
    return *this;
  }

  template <RangeType R> double operator*(const Bounded<R> &other) const {
    return val * static_cast<double>(other);
  }
  template <RangeType R> Bounded &operator*=(const Bounded<R> &other) {
    static_assert(Range::template spans<R>(),
                  "Source range is wider than target range");

    val *= static_cast<double>(other);
    validate();
    return *this;
  }
  Bounded &operator*=(double other) {
    val *= other;
    validate();
    return *this;
  }

  template <RangeType R> double operator/(const Bounded<R> &other) const {
    return val / static_cast<double>(other);
  }
  template <RangeType R> Bounded &operator/=(const Bounded<R> &other) {
    val /= static_cast<double>(other);
    validate();
    return *this;
  }
  Bounded &operator/=(double other) {
    val /= other;
    validate();
    return *this;
  }
};

/// @brief The range from zero to infinity, \f([0,\infty)\f)
struct NonNegativeRange : Range<ClosedLowerBound<NonNegativeRange>,
                                OpenUpperBound<NonNegativeRange>> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = std::numeric_limits<double>::infinity();
};
/// @brief Floats constraint to \f([0,\infty)\f)
using NonNegative = Bounded<NonNegativeRange>;

/// @brief The unit range \f([0,1]\f)
struct UnitRange
    : Range<ClosedLowerBound<UnitRange>, ClosedUpperBound<UnitRange>> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = 1.0;
};
/// @brief Floats constrained to \f([0,1]\f)
using Unit = Bounded<UnitRange>;
} // namespace range
