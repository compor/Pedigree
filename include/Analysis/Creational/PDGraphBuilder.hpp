//
//
//

#ifndef PEDIGREE_PDGRAPHBUILDER_HPP
#define PEDIGREE_PDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "Analysis/Graphs/DependenceGraphs.hpp"

#include "Analysis/Graphs/PDGraph.hpp"

#include "Analysis/Graphs/DDGraph.hpp"

#include "Analysis/Graphs/CDGraph.hpp"

#include "Analysis/Graphs/MDGraph.hpp"

#include "Support/GraphConverter.hpp"

#include "Support/Utils/UnitConverters.hpp"

#include <cassert>
// using assert

namespace llvm {
class Function;
} // namespace llvm end

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
  PDGraphBuilder(const CDGraph &CDG, const DDGraph &DDG, const MDGraph &MDG)
      : CDG(CDG), DDG(DDG), MDG(MDG) {}

  void build(PDGraph &PDG) const {
    InstructionDependenceGraph instCDG;
    Convert(CDG, instCDG, BlockToInstructionUnitConverter{});

    build(&instCDG, &PDG);
    build(&DDG, &PDG);
    build(&MDG, &PDG);
  }
};

} // namespace pedigree end

#endif // header
