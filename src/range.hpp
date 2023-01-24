#pragma once

#include <concepts>
#include <cstddef>

namespace range {

template <typename B, bool is_open> struct LowerBound;
template <typename B, bool is_open> struct UpperBound;

template <typename T>
concept LBType = requires(T x) {
                   { LowerBound{x} } -> std::same_as<T>;
                 };
template <typename T>
concept UBType = requires(T x) {
                   { UpperBound{x} } -> std::same_as<T>;
                 };

template <LBType L, UBType U> struct Range;

template <typename T>
concept RangeType = requires(T x) {
                      { true } -> std::convertible_to<bool>;
                    };

template <typename B, bool is_open> struct LowerBound {
  static constexpr double lower_bound = B::lower_bound;
  static constexpr bool lower_open = is_open;
  static constexpr bool below(double val) {
    return lower_open ? lower_bound < val : lower_bound <= val;
  }
  template <LBType Other> static constexpr bool below() {
    if constexpr (lower_open && !Other::lower_open) {
      return lower_bound < Other::lower_bound;
    } else {
      return lower_bound <= Other::lower_bound;
    }
  }
};
template <typename B> using OpenLowerBound = LowerBound<B, true>;
template <typename B> using ClosedLowerBound = LowerBound<B, false>;

template <typename B, bool is_open> struct UpperBound {
  static constexpr double upper_bound = B::upper_bound;
  static constexpr bool upper_open = is_open;
  static constexpr bool above(double val) {
    return upper_open ? val < upper_bound : val <= upper_bound;
  }
  template <UBType Other> static constexpr bool above() {
    if constexpr (upper_open && !Other::upper_open) {
      return upper_bound > Other::upper_bound;
    } else {
      return upper_bound >= Other::upper_bound;
    }
  }
};
template <typename B> using OpenUpperBound = UpperBound<B, true>;
template <typename B> using ClosedUpperBound = UpperBound<B, false>;

template <LBType L, UBType U> struct Range {
  using lower = L;
  using upper = U;
  static constexpr bool in_range(double val) {
    return lower::below(val) && upper::above(val);
  }
  template <RangeType R> static constexpr bool spans() {
    return lower::template below<typename R::lower>() &&
           upper::template above<typename R::upper>();
  }
};

} // namespace range