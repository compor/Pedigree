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

    for (const auto &node : GT::nodes(FromGraph)) {
      auto src = ToGraph->getOrInsertNode(node->get());

      for (const auto &child : GT::children(node)) {
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
