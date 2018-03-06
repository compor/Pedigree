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

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include "llvm/ADT/SmallPtrSet.h"
// using llvm::SmallPtrSet

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

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

  void traverseBottomUp(llvm::SmallVectorImpl<BlockT *> &Traversal,
                        const DomTreeT &DT, const DomTreeNodeT *Node) {
    constexpr size_t N = 32;
    llvm::SmallVector<BlockT *, N> workList;
    llvm::SmallPtrSet<BlockT *, N> visited;

    workList.push_back(Node->getBlock());

    while (!workList.empty()) {
      auto &top = *workList.rbegin();
      if (!visited.count(top)) {
        visited.insert(top);
        for (const auto &e : llvm::make_range(BlockTraits::child_begin(top),
                                              BlockTraits::child_end(top)))
          if (!visited.count(e))
            workList.push_back(e);
      } else {
        Traversal.push_back(top);
        workList.pop_back();
      }
    }
  }

  const DomSetType &calculate(const DomTreeT &DT, const DomTreeNodeT *Node) {
    return {};
  }
};

} // namespace pedigree end

#endif // header
