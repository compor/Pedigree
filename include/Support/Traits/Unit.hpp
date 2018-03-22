//
//
//

#ifndef PEDIGREE_TRAITS_UNIT_HPP
#define PEDIGREE_TRAITS_UNIT_HPP

#include "Config.hpp"

namespace llvm {
class Instruction;
class BasicBlock;
class Function;
} // namespace llvm end

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

template <> struct unit_traits<llvm::Instruction *> {
  using unit_category = instruction_unit_tag;
};

template <> struct unit_traits<llvm::BasicBlock> {
  using unit_category = basicblock_unit_tag;
};

template <> struct unit_traits<llvm::BasicBlock *> {
  using unit_category = basicblock_unit_tag;
};

template <> struct unit_traits<llvm::Function> {
  using unit_category = function_unit_tag;
};

template <> struct unit_traits<llvm::Function *> {
  using unit_category = function_unit_tag;
};

//

template <typename FromCategoryT, typename ToCategoryT>
struct is_unit_adaptable {
  static constexpr bool value = false;
};

template <>
struct is_unit_adaptable<basicblock_unit_tag, instruction_unit_tag> {
  static constexpr bool value = true;
};

} // namespace pedigree end

#endif // header
