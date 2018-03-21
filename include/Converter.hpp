//
//
//

#ifndef PEDIGREE_CONVERTER_HPP
#define PEDIGREE_CONVERTER_HPP

#include "Config.hpp"

#include "traits/Unit.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

namespace pedigree {

// TODO move to a place for handy utilities
template <> struct unit_conversion<llvm::BasicBlock *, llvm::Instruction *> {
  static llvm::Instruction *convert(const llvm::BasicBlock *From) {
    auto *from = const_cast<llvm::BasicBlock *>(From);
    return from->getTerminator();
  }
};

template <
    typename FromNodeT, typename ToNodeT,
    typename UCTraits = unit_conversion<typename FromNodeT::UnderlyingType,
                                        typename ToNodeT::UnderlyingType>>
void Convert(const GenericDependenceGraph<FromNodeT> &From,
             GenericDependenceGraph<ToNodeT> &To) {
  using FromUnderlyingT = typename FromNodeT::UnderlyingType;
  using ToUnderlyingT = typename ToNodeT::UnderlyingType;

  static_assert(is_unit_adaptable<
                    typename unit_traits<FromUnderlyingT>::unit_category,
                    typename unit_traits<ToUnderlyingT>::unit_category>::value,
                "Graph node undelying units are not adaptable!");

  for (auto &e : From) {
    auto src = To.getOrInsertNode(UCTraits::convert(e.first));

    for (auto &k : *(e.second))
      ;
  }
}

} // namespace pedigree end

#endif // header
