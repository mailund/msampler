#pragma once

#include <limits>

namespace bounded {

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

  template <typename R> Bounded(Bounded<R> &other) : val(other.val) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");
  }
  template <typename R> Bounded(Bounded<R> &&other) : val(other.val) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");
  }

  template <typename R> Bounded &operator=(Bounded<R> &other) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");
    val = other.val;
  }
  template <typename R> Bounded &operator=(Bounded<R> &&other) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");
    val = other.val;
  }

  operator double() const { return val; }
  template <typename R> double operator+(const Bounded<R> &other) const {
    return val + other.val;
  }
  template <typename R> Bounded &operator+=(const Bounded<R> &other) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");

    val += other.val;
    validate();
    return *this;
  }
  Bounded &operator+=(double other) {
    val += other;
    validate();
    return *this;
  }

  template <typename R> double operator-(const Bounded<R> &other) const {
    return val - other.val;
  }
  template <typename R> Bounded &operator-=(const Bounded<R> &other) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");

    val -= other.val;
    validate();
    return *this;
  }
  Bounded &operator-=(double other) {
    val -= other;
    validate();
    return *this;
  }

  template <typename R> double operator*(const Bounded<R> &other) const {
    return val * other.val;
  }
  template <typename R> Bounded &operator*=(const Bounded<R> &other) {
    static_assert(Range::template is_subrange<R>(),
                  "Source range is wider than target range");

    val *= other.val;
    validate();
    return *this;
  }
  Bounded &operator*=(double other) {
    val *= other;
    validate();
    return *this;
  }

  template <typename R> double operator/(const Bounded<R> &other) const {
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

template <typename Bounds, bool IsOpen> struct LowerBound {
  static constexpr double lower_bound = Bounds::lower_bound;
  static constexpr bool is_open = IsOpen;
  static constexpr bool in_bounds(double val) {
    if constexpr (is_open)
      return lower_bound < val;
    else
      return lower_bound <= val;
  }
  template <typename Other> static constexpr bool below() {
    if constexpr (Other::lower_bound < lower_bound)
      return true;
    if constexpr (Other::lower_bound == lower_bound)
      return Other::is_open || !is_open;
    return false;
  }
};
template <typename Bounds, bool IsOpen> struct UpperBound {
  static constexpr double upper_bound = Bounds::upper_bound;
  static constexpr bool is_open = IsOpen;
  static constexpr bool in_bounds(double val) {
    if constexpr (is_open)
      return val < upper_bound;
    else
      return val <= upper_bound;
  }
  template <typename Other> static constexpr bool above() {
    if constexpr (Other::upper_bound > upper_bound)
      return true;
    if constexpr (Other::upper_bound == upper_bound)
      return Other::is_open || !is_open;
    return false;
  }
};

/// @brief Specify a range of floats
/// @tparam Bounds Specifies the lower and upper bound of the range
///
/// Using Bounds is a hack to get around that we can't use floats
/// as template parameters. (C++20 does permit it, but clang doesn't
/// implement it yet).
///
/// @tparam LIsOpen Determine if the left/lower point is open
/// @tparam RIsOpen Determine if the right/upper point is open
template <typename Bounds, bool LIsOpen, bool RIsOpen> struct Range {
  using LowerBound = LowerBound<Bounds, LIsOpen>;
  using UpperBound = UpperBound<Bounds, RIsOpen>;
  static constexpr bool in_range(double v) {
    return LowerBound::in_bounds(v) && UpperBound::in_bounds(v);
  }
  template <typename R> static constexpr bool is_subrange() {
    return LowerBound::template below<typename R::LowerBound>() &&
           UpperBound::template above<typename R::UpperBound>();
  }
};

/// @brief The range from zero to infinity, \f([0,\infty)\f)
struct NonNegativeRange : Range<NonNegativeRange, false, true> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = std::numeric_limits<double>::infinity();
};
/// @brief Floats constraint to \f([0,\infty)\f)
using NonNegative = Bounded<NonNegativeRange>;

/// @brief The unit range \f([0,1]\f)
struct UnitRange : Range<UnitRange, false, false> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = 1.0;
};
/// @brief Floats constrained to \f([0,1]\f)
using Unit = Bounded<UnitRange>;

} // namespace bounded
