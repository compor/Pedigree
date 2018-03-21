//
//
//

#ifndef PEDIGREE_CONVERTER_HPP
#define PEDIGREE_CONVERTER_HPP

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

template <typename UnitT> struct unit_traits {
  using unit_category = typename UnitT::unit_category;
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

template <typename FromCategoryT, typename ToCategoryT>
struct is_unit_adaptable {
  static constexpr bool value = false;
};

template <>
struct is_unit_adaptable<basicblock_unit_tag, instruction_unit_tag> {
  static constexpr bool value = true;
};

template <typename FromNodeT, typename ToNodeT>
void Convert(const GenericDependenceGraph<FromNodeT> &From,
             GenericDependenceGraph<ToNodeT> &To) {
  using FromUnderT = typename FromNodeT::UnderlyingType;
  using ToUnderT = typename ToNodeT::UnderlyingType;

  static_assert(
      is_unit_adaptable<typename unit_traits<FromUnderT>::unit_category,
                        typename unit_traits<ToUnderT>::unit_category>::value,
      "Graph node undelying units are not adaptable!");
}

} // namespace pedigree end

#endif // header
