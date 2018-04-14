//
//
//

#ifndef PEDIGREE_UNITTRAITS_HPP
#define PEDIGREE_UNITTRAITS_HPP

#include "Config.hpp"

#include "Support/Traits/UnitTraits.hpp"

#include "Support/Utils/UniformObjectAccess.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstrTypes.h"
// using llvm::TerminatorInst

#include "llvm/Support/Casting.h"
// using llvm::dyn_cast

#include "llvm/Support/raw_ostream.h"
// using llvm::raw_string_ostream

#include <type_traits>
// using std::integral_constant
// using std::enable_if

#include <string>
// using std::string

namespace llvm {
class BasicBlock;
class Function;
} // namespace llvm

namespace pedigree {

struct instruction_unit_tag {};
struct basicblock_unit_tag {};
struct function_unit_tag {};

//

struct default_unit_traits {
  template <typename T> static std::string name(const T &Unit) {
    return ToObj(Unit).getName();
  }

  template <typename T> static std::string print(const T &Unit) {
    std::string s;
    llvm::raw_string_ostream os(s);
    ToObj(Unit).print(os);

    return os.str();
  }
};

//

template <typename UnderlyingT, typename _ = void> struct unit_traits {
  // using unit_category = typename UnderlyingT::unit_category;
  // static std::string name(const UnderlyingT &);
  // static std::string print(const UnderlyingT &);
};

template <typename UnderlyingT>
struct unit_traits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::Instruction, UnderlyingT>>>
    : default_unit_traits {
  using Base = default_unit_traits;

  using unit_category = instruction_unit_tag;

  template <typename T> static std::string name(const T &Unit) {
    auto *term = llvm::dyn_cast<llvm::TerminatorInst>(ToPtr(Unit));

    return term ? "term@" + ToObj(Unit).getParent()->getName().str()
                : ToObj(Unit).getName().str();
  }

  using Base::print;
};

template <typename UnderlyingT>
struct unit_traits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::BasicBlock, UnderlyingT>>>
    : default_unit_traits {
  using Base = default_unit_traits;

  using unit_category = basicblock_unit_tag;

  using Base::name;
  using Base::print;
};

template <typename UnderlyingT>
struct unit_traits<
    UnderlyingT,
    typename std::enable_if_t<is_unqual_same_v<llvm::Function, UnderlyingT>>>
    : default_unit_traits {
  using Base = default_unit_traits;

  using unit_category = function_unit_tag;

  using Base::name;
  using Base::print;
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
