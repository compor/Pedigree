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
// using std::is_const
// using std::remove_cv
// using std::remove_pointer
// using std::remove_reference
// using std::is_nothrow_move_constructible
// using std::is_nothrow_move_assignable

#include <iterator>
// using std::iterator_traits

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

template <typename T, typename U>
using is_unqual_same =
    is_same<T, typename std::remove_cv_t<remove_pointer_or_reference_t<U>>>;

template <typename T, typename U>
constexpr bool is_unqual_same_v = is_unqual_same<T, U>::value;

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
constexpr bool are_all_nothrow_move_constructible_v =
    are_all_nothrow_move_constructible<Args...>::value;

//

template <typename... Args>
struct are_all_nothrow_move_assignable
    : conjuction<std::is_nothrow_move_assignable<Args>::value...> {};

template <typename... Args>
constexpr bool are_all_nothrow_move_assignable_v =
    are_all_nothrow_move_assignable<Args...>::value;

//

template <typename T>
struct is_const_iterator
    : std::is_const<
          std::remove_pointer_t<typename std::iterator_traits<T>::pointer>> {};

template <typename T>
constexpr bool is_const_iterator_v = is_const_iterator<T>::value;

} // namespace pedigree

#endif // header
