//
//
//

#ifndef PEDIGREE_UNITTRAITS_HPP
#define PEDIGREE_UNITTRAITS_HPP

#include "Config.hpp"

#include "Support/Traits/UnitTraits.hpp"

#include <type_traits>
// using std::integral_constant
// using std::enable_if

namespace llvm {
class Instruction;
class BasicBlock;
class Function;
} // namespace llvm

namespace pedigree {

struct instruction_unit_tag {};
struct basicblock_unit_tag {};
struct function_unit_tag {};

//

template <typename UnderlyingT, typename _ = void> struct unit_traits {
  // using unit_category = typename UnderlyingT::unit_category;
};

template <typename UnderlyingT>
struct unit_traits<UnderlyingT, typename std::enable_if_t<is_unqual_same_v<
                                    llvm::Instruction, UnderlyingT>>> {
  using unit_category = instruction_unit_tag;
};

template <typename UnderlyingT>
struct unit_traits<UnderlyingT, typename std::enable_if_t<is_unqual_same_v<
                                    llvm::BasicBlock, UnderlyingT>>> {
  using unit_category = basicblock_unit_tag;
};

template <typename UnderlyingT>
struct unit_traits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::Function, UnderlyingT>>> {
  using unit_category = function_unit_tag;
};

//

namespace detail {

template <typename FromCategoryT, typename ToCategoryT>
struct is_unit_convertible_impl : std::integral_constant<bool, false> {};

template <>
struct is_unit_convertible_impl<basicblock_unit_tag, instruction_unit_tag>
    : std::integral_constant<bool, true> {};

} // namespace detail

template <typename FromT, typename ToT>
struct is_unit_convertible : detail::is_unit_convertible_impl<
                                 typename unit_traits<FromT>::unit_category,
                                 typename unit_traits<ToT>::unit_category> {};

template <typename FromT, typename ToT>
constexpr bool is_unit_convertible_v = is_unit_convertible<FromT, ToT>::value;

} // namespace pedigree

#endif // header
