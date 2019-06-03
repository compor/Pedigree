//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/ADT/GraphTraits.h"
// using llvm::GraphTraits
// using llvm::Inverse

#include "llvm/IR/Instructions.h"
// using llvm::UnreachableInst

#include "llvm/IR/CFG.h"
// using llvm::GraphTraits
// using llvm::pred_size

#include "llvm/IR/Dominators.h"
// using llvm::DominatorTreeBase
// using llvm::DominatorTreeNodeBase

#include "llvm/Analysis/DominanceFrontier.h"
// using llvm::DominanceFrontierBase

#include "llvm/ADT/PostOrderIterator.h"
// using llvm::post_order

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <algorithm>
// using std::copy

#include <cassert>
// using assert

#define DEBUG_TYPE "pedigree-postdom-frontier"

namespace pedigree {

namespace detail {

template <typename NodeT> decltype(auto) graph_parents(NodeT *BB) {
  using IGT = llvm::GraphTraits<llvm::Inverse<NodeT *>>;
  return llvm::make_range(IGT::child_begin(BB), IGT::child_end(BB));
}

} // namespace detail

template <typename BlockT>
class PostDominanceFrontierBase
#if (LLVM_VERSION_MAJOR >= 5)
    : public llvm::DominanceFrontierBase<BlockT, true>
#else
    : public llvm::DominanceFrontierBase<BlockT>
#endif
{
public:
#if (LLVM_VERSION_MAJOR >= 5)
  using Base = llvm::DominanceFrontierBase<BlockT, true>;
  using DomTreeT = llvm::DominatorTreeBase<BlockT, true>;
#else
  using Base = llvm::DominanceFrontierBase<BlockT>;
  using DomTreeT = llvm::DominatorTreeBase<BlockT>;
#endif
  using DomTreeNodeT = llvm::DomTreeNodeBase<BlockT>;
  using DomSetType = typename Base::DomSetType;
  using DomSetMapType = typename Base::DomSetMapType;

#if (LLVM_VERSION_MAJOR >= 5)
  PostDominanceFrontierBase() : Base() {}
#else
  PostDominanceFrontierBase() : Base(true) {}
#endif

  void analyze(DomTreeT &DT) {
    const auto &roots = DT.getRoots();

    this->Roots.resize(roots.size());
    std::copy(roots.begin(), roots.end(), this->Roots.begin());

    calculate(DT);
  }

protected:
  BlockT *getSingleRoot() const {
    if (this->Roots.size() == 1) {
      return this->Roots[0];
    }

    BlockT *root = nullptr;

    for (auto i = 0; i < this->Roots.size(); ++i) {
      root = this->Roots[i];
      if (root && llvm::pred_size(root)) {
        return root;
      }
    }

    // NOTE this is an ugly catch-all solution
    if (!root) {
      root = &this->Roots[0]->getParent()->getEntryBlock();
    }

    return root;
  }

  const DomSetType &calculate(const DomTreeT &DT) {
    this->Frontiers.clear();

    auto *blockRoot = getSingleRoot();
    assert(blockRoot && "Only one entry block for post domfronts!");

    LLVM_DEBUG(llvm::dbgs()
                   << "selected postdom tree root block of function '"
                   << blockRoot->getParent()->getName() << "' with terminator: "
                   << *blockRoot->getTerminator() << '\n';);

    auto *root = DT[blockRoot];
    const auto &traversal = llvm::post_order(root);

    for (const auto &e : traversal)
      this->Frontiers[e->getBlock()] = {};

    for (const auto &e : traversal) {
      const auto &curBlock = e->getBlock();

      // DF-local
      for (const auto &c : detail::graph_parents(curBlock))
        if (DT[c]->getIDom()->getBlock() != curBlock)
          this->Frontiers[curBlock].insert(c);

      // DF-up
      for (const auto &d : *DT[curBlock])
        for (const auto &f : this->Frontiers[d->getBlock()])
          if (DT[f]->getIDom()->getBlock() != curBlock)
            this->Frontiers[curBlock].insert(f);
    }

    return this->Frontiers[root->getBlock()];
  }
};

} // namespace pedigree

#undef DEBUG_TYPE

