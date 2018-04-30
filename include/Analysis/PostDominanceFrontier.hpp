//
//
//

#ifndef PEDIGREE_POSTDOMINANCEFRONTIER_HPP
#define PEDIGREE_POSTDOMINANCEFRONTIER_HPP

#include "llvm/Config/llvm-config.h"
// version macros

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

#include <algorithm>
// using std::copy

#include <cassert>
// using assert

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

    assert(roots.size() == 1 && "Only one entry block for post domfronts!");

    this->Roots.resize(roots.size());
    std::copy(roots.begin(), roots.end(), this->Roots.begin());
    calculate(DT);
  }

  void traverseDFSPostOrder(llvm::SmallVectorImpl<BlockT *> &Traversal,
                            const DomTreeT &DT) const {
    constexpr size_t N = 32;
    llvm::SmallVector<BlockT *, N> workList;
    llvm::SmallPtrSet<BlockT *, N> visited;

    for (auto &e : DT.getRoots())
      workList.push_back(e);

    while (!workList.empty()) {
      auto &top = *workList.rbegin();
      if (!visited.count(top)) {
        visited.insert(top);
        for (const auto &c : detail::graph_parents(top)) {
          if (!visited.count(c)) {
            workList.push_back(c);
          }
        }
      } else {
        Traversal.push_back(top);
        workList.pop_back();
      }
    }
  }

protected:
  const DomSetType &calculate(const DomTreeT &DT) {
    this->Frontiers.clear();
    auto *Root = DT[this->Roots[0]];

    llvm::SmallVector<BlockT *, 32> traversal;
    traverseDFSPostOrder(traversal, DT);

    for (auto &e : traversal)
      this->Frontiers[e] = {};

    for (auto &e : traversal) {
      // DF-local
      for (const auto &c : detail::graph_parents(e))
        if (DT[c]->getIDom()->getBlock() != e)
          this->Frontiers[e].insert(c);

      // DF-up
      for (auto &d : *DT[e])
        for (auto &f : this->Frontiers[d->getBlock()])
          if (DT[f]->getIDom()->getBlock() != e)
            this->Frontiers[e].insert(f);
    }

    return this->Frontiers[Root->getBlock()];
  }
};

} // namespace pedigree

#endif // header
