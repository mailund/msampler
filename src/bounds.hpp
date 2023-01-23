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
    // FIXME: handle open/close trait
    static_assert(R::lower_bound <= Range::lower_bound,
                  "Assigning from a wider to a narrower range");
    static_assert(R::upper_bound >= Range::upper_bound,
                  "Assigning from a wider to a narrower range");
  }
  template <typename R> Bounded(Bounded<R> &&other) : val(other.val) {
    // FIXME: handle open/close trait
    static_assert(R::lower_bound <= Range::lower_bound,
                  "Assigning from a wider to a narrower range");
    static_assert(R::upper_bound >= Range::upper_bound,
                  "Assigning from a wider to a narrower range");
  }

  template <typename R> Bounded &operator=(Bounded<R> &other) {
    // FIXME: handle open/close trait
    static_assert(R::lower_bound <= Range::lower_bound,
                  "Assigning from a wider to a narrower range");
    static_assert(R::upper_bound >= Range::upper_bound,
                  "Assigning from a wider to a narrower range");
    val = other.val;
  }
  template <typename R> Bounded &operator=(Bounded<R> &&other) {
    // FIXME: handle open/close trait
    static_assert(R::lower_bound <= Range::lower_bound,
                  "Assigning from a wider to a narrower range");
    static_assert(R::upper_bound >= Range::upper_bound,
                  "Assigning from a wider to a narrower range");
    val = other.val;
  }

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

  template <typename R> double operator-(const Bounded<R> &other) const {
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

  template <typename R> double operator*(const Bounded<R> &other) const {
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

/// @brief Specify a range of floats
/// @tparam Bounds Specifies the lower and upper bound of the range
///
/// Using Bounds is a hack to get around that we can't use floats
/// as template parameters. (C++20 does permit it, but clang doesn't
/// implement it yet).
///
/// @tparam L Determine if the left/lower point is included, `[` or not `(`
/// @tparam U Determine if the right/upper point is included, `]` or not `)`
template <typename Bounds, char L, char U> struct Range {
  static constexpr double lower_bound = Bounds::lower_bound;
  static constexpr double upper_bound = Bounds::upper_bound;
  static constexpr bool lower_closed = L == '[';
  static constexpr bool upper_closed = L == '[';
  static constexpr bool above_lower(double v) {
    return (L == '[') ? lower_bound <= v : lower_bound < v;
  }
  static constexpr bool below_upper(double v) {
    return (U == ']') ? v <= upper_bound : v < upper_bound;
  }
  static constexpr bool in_range(double v) {
    return above_lower(v) && below_upper(v);
  }
};

/// @brief The range from zero to infinity, \f([0,\infty)\f)
struct NonNegativeRange : Range<NonNegativeRange, '[', ')'> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = std::numeric_limits<double>::infinity();
};
/// @brief Floats constraint to \f([0,\infty)\f)
using NonNegative = Bounded<NonNegativeRange>;

/// @brief The unit range \f([0,1]\f)
struct UnitRange : Range<UnitRange, '[', ']'> {
  static constexpr double lower_bound = 0.0;
  static constexpr double upper_bound = 1.0;
};
/// @brief Floats constrained to \f([0,1]\f)
using Unit = Bounded<UnitRange>;

} // namespace bounded
