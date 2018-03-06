//
//
//

#ifndef PEDIGREE_POSTDOMINANCEFRONTIER_HPP
#define PEDIGREE_POSTDOMINANCEFRONTIER_HPP

#include "llvm/ADT/GraphTraits.h"
// using llvm::GraphTraits
// using llvm::Inverse

#include "llvm/IR/CFG.h"
// using llvm::GraphTraits

#include "llvm/IR/Dominators.h"
// using llvm::DominatorTreeBase
// using llvm::DominatorTreeNodeBase

#include "llvm/Analysis/DominanceFrontier.h"
// using llvm::DominanceFrontierBase

#include <cassert>
// using assert

namespace pedigree {

template <typename BlockT>
class PostDominanceFrontierBase : public llvm::DominanceFrontierBase<BlockT> {
  using BlockTraits = llvm::GraphTraits<llvm::Inverse<BlockT *>>;

public:
  using Base = llvm::DominanceFrontierBase<BlockT>;
  using DomTreeT = llvm::DominatorTreeBase<BlockT>;
  using DomTreeNodeT = llvm::DomTreeNodeBase<BlockT>;
  using DomSetType = typename Base::DomSetType;

  PostDominanceFrontierBase() : Base(true) {}

  void analyze(DomTreeT &DT) {
    this->Roots = DT.getRoots();
    assert(this->Roots.size() == 1 &&
           "Only one entry block for post domfronts!");

    calculate(DT, DT[this->Roots[0]]);
  }

  const DomSetType &calculate(const DomTreeT &DT, const DomTreeNodeT &Node) {
    return {};
  }
};

} // namespace pedigree end

#endif // header
