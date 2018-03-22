//
//
//

#ifndef PEDIGREE_CONVERTER_HPP
#define PEDIGREE_CONVERTER_HPP

#include "Config.hpp"

#include "Support/Traits/Unit.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace llvm {
class Instruction;
}

namespace pedigree {

// TODO move to a place for handy utilities
template <> struct unit_adaptor<llvm::BasicBlock *, llvm::Instruction *> {
  static llvm::Instruction *adapt(const llvm::BasicBlock *From) {
    auto *from = const_cast<llvm::BasicBlock *>(From);
    return from->getTerminator();
  }
};

template <typename FromNodeT, typename ToNodeT,
          typename UATraits = unit_adaptor<typename FromNodeT::UnderlyingType,
                                           typename ToNodeT::UnderlyingType>>
void Adapt(const GenericDependenceGraph<FromNodeT> &From,
           GenericDependenceGraph<ToNodeT> &To) {
  using FromUnderlyingT = typename FromNodeT::UnderlyingType;
  using ToUnderlyingT = typename ToNodeT::UnderlyingType;

  static_assert(is_unit_adaptable<
                    typename unit_traits<FromUnderlyingT>::unit_category,
                    typename unit_traits<ToUnderlyingT>::unit_category>::value,
                "Graph node undelying units are not adaptable!");

  for (auto &e : From) {
    auto src = To.getOrInsertNode(UATraits::adapt(e.first));

    for (auto &k : *(e.second)) {
      auto adapted = UATraits::adapt(k.first->get());
      auto dst = To.getOrInsertNode(adapted);
      src->addDependentNode(dst, {});
    }
  }
}

} // namespace pedigree end

#endif // header
