//
//
//

#ifndef PEDIGREE_PDGraphBUILDER_HPP
#define PEDIGREE_PDGraphBUILDER_HPP

#include "Config.hpp"

#include "DependenceGraphs.hpp"

#include "PDGraph.hpp"

#include "DDGraph.hpp"

#include "CDGraph.hpp"

#include "MDGraph.hpp"

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

#include <cassert>
// using assert

namespace pedigree {

class PDGraphBuilder {
  const CDGraph &CDG;
  const DDGraph &DDG;
  const MDGraph &MDG;

  void build(const InstructionDependenceGraph *FromGraph,
             InstructionDependenceGraph *ToGraph) const {
    assert(FromGraph && "Graph pointer is empty!");
    assert(ToGraph && "Graph pointer is empty!");

    using GT = llvm::GraphTraits<decltype(FromGraph)>;

    for (const auto &node : llvm::make_range(GT::nodes_begin(FromGraph),
                                             GT::nodes_end(FromGraph))) {
      auto src = ToGraph->getOrInsertNode(node->get());

      for (const auto &child :
           llvm::make_range(GT::child_begin(node), GT::child_end(node))) {
        auto dst = ToGraph->getOrInsertNode(child->get());
        src->addDependentNode(dst, {});
      }
    }
  }

public:
  PDGraphBuilder(const CDGraph &Cdg, const DDGraph &Ddg, const MDGraph &Mdg)
      : CDG(Cdg), DDG(Ddg), MDG(Mdg) {}

  void build(PDGraph &PDG) const {
    build(&DDG, &PDG);
    build(&MDG, &PDG);
  }
};

} // namespace pedigree end

#endif // header
