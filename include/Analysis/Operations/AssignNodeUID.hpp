//
//
//

#ifndef PEDIGREE_DFSENUMERATE_HPP
#define PEDIGREE_DFSENUMERATE_HPP

#include "Config.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/CFG.h"

#include <memory>
// using std::addressof

#include <iterator>
// using std::iterator_traits

#include <type_traits>
// using std::is_pointer

#include <utility>
// using std::declval

namespace pedigree {

template <typename GraphT, typename IteratorT>
void AssignNodeUID(GraphT &G, IteratorT Begin, IteratorT End) {
  unsigned int i = 0;

  static_assert(!std::is_pointer<GraphT>::value,
                "Graph type cannot be a pointer!");

  std::for_each(Begin, End, [&](const auto &e) {
    auto node = G.getNode(e);

    if (node) {
      auto &info = (*node)->info();
      info.id = i;
    }

    ++i;
  });
}

} // namespace pedigree

#endif // header
