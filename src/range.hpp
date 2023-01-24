#pragma once

#include <concepts>
#include <cstddef>

namespace range {

struct Open;
struct Closed;
template <typename> struct open_trait;
template <> struct open_trait<Open> {
  static constexpr bool value = true;
};
template <> struct open_trait<Closed> {
  static constexpr bool value = false;
};

template <typename B, typename is_open> struct LowerBound;
template <typename B, typename is_open> struct UpperBound;

// clang-format off
template <typename T, template <typename...> class Template>
concept TemplateInstance = requires(T x) {
  // Can we call a function that takes a range with
  // T? If so, T is derived from a Range.
  []<typename... args>(Template<args...> &) {}(x);
};
// clang-format on

template <typename T>
concept LBType = TemplateInstance<T, LowerBound>;
template <typename T>
concept UBType = TemplateInstance<T, UpperBound>;

template <typename B, typename is_open> struct LowerBound {
  static constexpr double lower_bound = B::lower_bound;
  static constexpr bool lower_open = open_trait<is_open>::value;
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
template <typename B> using OpenLowerBound = LowerBound<B, Open>;
template <typename B> using ClosedLowerBound = LowerBound<B, Closed>;

template <typename B, typename is_open> struct UpperBound {
  static constexpr double upper_bound = B::upper_bound;
  static constexpr bool upper_open = open_trait<is_open>::value;
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
template <typename B> using OpenUpperBound = UpperBound<B, Open>;
template <typename B> using ClosedUpperBound = UpperBound<B, Closed>;

template <LBType L, UBType U> struct Range;

template <typename T>
concept RangeType = TemplateInstance<T, Range>;

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