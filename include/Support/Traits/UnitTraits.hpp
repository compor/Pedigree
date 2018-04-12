//
//
//

#ifndef PEDIGREE_UNITTRAITS_HPP
#define PEDIGREE_UNITTRAITS_HPP

#include "Config.hpp"

#include <type_traits>
// using std::integral_constant

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

template <typename UnderlyingT> struct unit_traits {
  using unit_category = typename UnderlyingT::unit_category;
};

template <> struct unit_traits<llvm::Instruction> {
  using unit_category = instruction_unit_tag;
};

template <> struct unit_traits<const llvm::Instruction> {
  using unit_category = instruction_unit_tag;
};

template <> struct unit_traits<llvm::Instruction *> {
  using unit_category = instruction_unit_tag;
};

template <> struct unit_traits<const llvm::Instruction *> {
  using unit_category = instruction_unit_tag;
};

template <> struct unit_traits<llvm::BasicBlock> {
  using unit_category = basicblock_unit_tag;
};

template <> struct unit_traits<const llvm::BasicBlock> {
  using unit_category = basicblock_unit_tag;
};

template <> struct unit_traits<llvm::BasicBlock *> {
  using unit_category = basicblock_unit_tag;
};

template <> struct unit_traits<const llvm::BasicBlock *> {
  using unit_category = basicblock_unit_tag;
};

template <> struct unit_traits<llvm::Function> {
  using unit_category = function_unit_tag;
};

template <> struct unit_traits<const llvm::Function> {
  using unit_category = function_unit_tag;
};

template <> struct unit_traits<llvm::Function *> {
  using unit_category = function_unit_tag;
};

template <> struct unit_traits<const llvm::Function *> {
  using unit_category = function_unit_tag;
};

//

template <typename FromCategoryT, typename ToCategoryT>
struct is_unit_convertible : std::integral_constant<bool, false> {};

template <>
struct is_unit_convertible<basicblock_unit_tag, instruction_unit_tag>
    : std::integral_constant<bool, true> {};

} // namespace pedigree

#endif // header
