#pragma once

#include <concepts>
#include <cstddef>
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
template <typename T>
concept Bound = requires {
                  { T::value } -> std::convertible_to<double>;
                  { T::is_open } -> std::convertible_to<bool>;
                };
template <typename T>
concept LowerBound = requires {
                       Bound<T>;
                       { T::below(0.0) } -> std::convertible_to<bool>;
                       //{ T::template below() } ->
                       // std::convertible_to<bool>;
                     };
template <typename T>
concept UpperBound = requires {
                       Bound<T>;
                       { T::above(0.0) } -> std::convertible_to<bool>;
                       //{ T::template above() } ->
                       // std::convertible_to<bool>;
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
template <typename B, bool LIsOpen, bool RIsOpen> struct Range {
  struct lower_bound_t {
    static constexpr double value = B::lower_bound;
    static constexpr bool is_open = LIsOpen;
    static constexpr bool below(double val) {
      return is_open ? value < val : value <= val;
    }
    template <Bound Other> static constexpr bool below() {
      if constexpr (value < Other::value)
        return true;
      if constexpr (Other::value == value)
        return Other::is_open || !is_open;
      return false;
    }
  };

  struct upper_bound_t {
    static constexpr double value = B::upper_bound;
    static constexpr bool is_open = RIsOpen;
    static constexpr bool above(double val) {
      return is_open ? val < value : val <= value;
    }
    template <Bound Other> static constexpr bool above() {
      if constexpr (value > Other::value)
        return true;
      if constexpr (Other::value == value)
        return Other::is_open || !is_open;
      return false;
    }
  };

  static constexpr bool in_range(double v) {
    return lower_bound_t::below(v) && upper_bound_t::above(v);
  }
  template <typename R> static constexpr bool is_subrange() {
    return lower_bound_t::template below<typename R::lower_bound_t>() &&
           upper_bound_t::template above<typename R::upper_bound_t>();
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
