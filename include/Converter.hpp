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

struct BlockToInstructionUnitAdaptor {
  llvm::Instruction *operator()(const llvm::BasicBlock *From) {
    auto *from = const_cast<llvm::BasicBlock *>(From);
    return from->getTerminator();
  }
};

template <typename FromNodeT, typename ToNodeT, typename AdaptOperation>
void Adapt(const GenericDependenceGraph<FromNodeT> &From,
           GenericDependenceGraph<ToNodeT> &To, AdaptOperation Adapt) {
  using FromUnderlyingT = typename FromNodeT::UnderlyingType;
  using ToUnderlyingT = typename ToNodeT::UnderlyingType;

  static_assert(is_unit_adaptable<
                    typename unit_traits<FromUnderlyingT>::unit_category,
                    typename unit_traits<ToUnderlyingT>::unit_category>::value,
                "Graph node undelying units are not adaptable!");

  for (auto &e : From) {
    auto src = To.getOrInsertNode(Adapt(e.first));

    for (auto &k : *(e.second)) {
      auto adapted = Adapt(k.first->get());
      auto dst = To.getOrInsertNode(adapted);
      src->addDependentNode(dst, {});
    }
  }
}

} // namespace pedigree end

#endif // header
