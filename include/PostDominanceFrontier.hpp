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

  auto children(BlockT *BB) const {
    return llvm::make_range(BlockTraits::child_begin(BB),
                            BlockTraits::child_end(BB));
  }

public:
  using Base = llvm::DominanceFrontierBase<BlockT>;
  using DomTreeT = llvm::DominatorTreeBase<BlockT>;
  using DomTreeNodeT = llvm::DomTreeNodeBase<BlockT>;
  using DomSetType = typename Base::DomSetType;
  using DomSetMapType = typename Base::DomSetMapType;

  PostDominanceFrontierBase() : Base(true) {}

  void analyze(DomTreeT &DT) {
    this->Roots = DT.getRoots();
    assert(this->Roots.size() == 1 &&
           "Only one entry block for post domfronts!");

    calculate(DT, DT[this->Roots[0]]);
  }

  void traverseDFSPostOrder(llvm::SmallVectorImpl<BlockT *> &Traversal,
                            const DomTreeT &DT,
                            const DomTreeNodeT *Node) const {
    constexpr size_t N = 32;
    llvm::SmallVector<BlockT *, N> workList;
    llvm::SmallPtrSet<BlockT *, N> visited;

    workList.push_back(Node->getBlock());

    while (!workList.empty()) {
      auto &top = *workList.rbegin();
      if (!visited.count(top)) {
        visited.insert(top);
        for (const auto &c : children(top))
          if (!visited.count(c))
            workList.push_back(c);
      } else {
        Traversal.push_back(top);
        workList.pop_back();
      }
    }
  }

  const DomSetType &calculate(const DomTreeT &DT, const DomTreeNodeT *Node) {
    this->Frontiers.clear();

    llvm::SmallVector<BlockT *, 32> traversal;
    traverseDFSPostOrder(traversal, DT, Node);

    // DF-local
    for (auto &e : traversal)
      for (const auto &c : children(e))
        if (DT[c]->getIDom()->getBlock() != e)
          this->Frontiers[e].insert(c);

    // DF-up
    for (auto &e : traversal)
      for (auto &c : *DT[e])
        if (c->getIDom()->getBlock() != e)
          this->Frontiers[e].insert(e);

    return this->Frontiers[Node->getBlock()];
  }
};

} // namespace pedigree end

#endif // header
