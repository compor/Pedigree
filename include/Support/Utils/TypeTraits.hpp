//
//
//

#ifndef PEDIGREE_TYPETRAITS_HPP
#define PEDIGREE_TYPETRAITS_HPP

#include <type_traits>
// using std::integral_constant
// using std::is_nothrow_move_constructible
// using std::is_nothrow_move_assignable

namespace pedigree {

template <bool... B> struct conjuction {};

template <bool B, bool... Rest>
struct conjuction<B, Rest...>
    : std::integral_constant<bool, B && conjuction<Rest...>::value> {};

template <bool B> struct conjuction<B> : std::integral_constant<bool, B> {};

//

template <typename T, typename... TArgs>
struct are_all_nothrow_move_constructible {
  static const bool value = std::is_nothrow_move_constructible<T>::value &&
                            are_all_nothrow_move_constructible<TArgs...>::value;

  constexpr bool operator()() const noexcept { return value; }
  constexpr operator bool() const noexcept { return value; }

  using value_type = bool;
  using type = are_all_nothrow_move_constructible;
};

template <typename T> struct are_all_nothrow_move_constructible<T> {
  static const bool value = std::is_nothrow_move_constructible<T>::value;

  constexpr bool operator()() const noexcept { return value; }
  constexpr operator bool() const noexcept { return value; }

  using value_type = bool;
  using type = are_all_nothrow_move_constructible;
};

//

template <typename T, typename... TArgs>
struct are_all_nothrow_move_assignable {
  static const bool value = std::is_nothrow_move_assignable<T>::value &&
                            are_all_nothrow_move_assignable<TArgs...>::value;

  constexpr bool operator()() const noexcept { return value; }
  constexpr operator bool() const noexcept { return value; }

  using value_type = bool;
  using type = are_all_nothrow_move_assignable;
};

template <typename T> struct are_all_nothrow_move_assignable<T> {
  static const bool value = std::is_nothrow_move_assignable<T>::value;

  constexpr bool operator()() const noexcept { return value; }
  constexpr operator bool() const noexcept { return value; }

  using value_type = bool;
  using type = are_all_nothrow_move_assignable;
};

} // namespace pedigree

#endif // PEDIGREE_TYPETRAITS_HPP
