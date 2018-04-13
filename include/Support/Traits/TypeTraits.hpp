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

template <typename... Args>
struct are_all_nothrow_move_constructible
    : conjuction<std::is_nothrow_move_constructible<Args>::value...> {};

template <typename... Args>
struct are_all_nothrow_move_assignable
    : conjuction<std::is_nothrow_move_assignable<Args>::value...> {};

} // namespace pedigree

#endif // PEDIGREE_TYPETRAITS_HPP
