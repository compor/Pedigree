//
//
//

#ifndef PEDIGREE_DFSENUMERATE_HPP
#define PEDIGREE_DFSENUMERATE_HPP

#include "Config.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/CFG.h"

#include "llvm/ADT/DepthFirstIterator.h"
// using llvm::depth_first

#include <memory>
// using std::addressof

#include <iterator>
// using std::iterator_traits

namespace pedigree {

template <typename GraphT, typename UnitT>
void DFSEnumerate(GraphT &G, const UnitT &Unit) {
  auto dfs = llvm::depth_first(std::addressof(const_cast<UnitT &>(Unit)));
  unsigned int i = 0;

  for (auto *e : dfs) {
    for (auto &k : *e) {
      auto node = G.getNode(std::addressof(k));

      if (node) {
        auto &info = (*node)->info();
        info.id = i;
      }

      ++i;
    }
  }
}

} // namespace pedigree

#endif // header
