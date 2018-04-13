//
//
//

#ifndef PEDIGREE_TYPETRAITS_HPP
#define PEDIGREE_TYPETRAITS_HPP

#include <type_traits>
// using std::integral_constant
// using std::conditional
// using std::is_same
// using std::is_pointer
// using std::remove_pointer
// using std::remove_reference
// using std::is_nothrow_move_constructible
// using std::is_nothrow_move_assignable

namespace pedigree {

template <typename T>
using remove_pointer_or_reference =
    std::conditional<std::is_pointer<T>::value,
                     typename std::remove_pointer<T>::type,
                     typename std::remove_reference<T>::type>;

template <typename T>
using remove_pointer_or_reference_t =
    typename remove_pointer_or_reference<T>::type;

//

template <typename T, typename U> using is_same = typename std::is_same<T, U>;

template <typename T, typename U>
constexpr bool is_same_v = std::is_same<T, U>::value;

//

template <bool... B> struct conjuction {};

template <bool B, bool... Rest>
struct conjuction<B, Rest...>
    : std::integral_constant<bool, B && conjuction<Rest...>::value> {};

template <bool B> struct conjuction<B> : std::integral_constant<bool, B> {};

//

template <typename... Args>
struct are_all_nothrow_move_constructible
    : conjuction<std::is_nothrow_move_constructible<Args>::value...> {};

template <typename... Args>
struct are_all_nothrow_move_assignable
    : conjuction<std::is_nothrow_move_assignable<Args>::value...> {};

} // namespace pedigree

#endif // PEDIGREE_TYPETRAITS_HPP
