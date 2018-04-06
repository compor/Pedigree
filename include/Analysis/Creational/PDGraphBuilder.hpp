//
//
//

#ifndef PEDIGREE_PDGRAPHBUILDER_HPP
#define PEDIGREE_PDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "Analysis/Graphs/DependenceGraphs.hpp"

#include "Analysis/Graphs/PDGraph.hpp"

#include <vector>
// using std::vector

#include <functional>
// using std::cref

#include <memory>
// using std::unique_ptr
// using std::make_unique

namespace pedigree {

class PDGraphBuilder {
  std::vector<std::reference_wrapper<const InstructionDependenceGraph>>
      componentGraphs;
  std::unique_ptr<PDGraph> Graph;
  bool LazyConstruction;

  void combine(InstructionDependenceGraph &ToGraph,
               const InstructionDependenceGraph &FromGraph) {
    assert(Graph && "Graph is null!");

    using GT = llvm::GraphTraits<decltype(&FromGraph)>;

    for (const auto &node : GT::nodes(&FromGraph)) {
      auto src = ToGraph.getOrInsertNode(node->get());

      for (const auto &child : GT::children(node)) {
        auto dst = ToGraph.getOrInsertNode(child->get());
        src->addDependentNode(dst, {});
      }
    }
  }

public:
  PDGraphBuilder() : LazyConstruction(false) {}

  PDGraphBuilder &addGraph(const InstructionDependenceGraph &FromGraph) {
    if (LazyConstruction)
      componentGraphs.emplace_back(std::cref(FromGraph));
    else {
      if (!Graph)
        Graph = std::make_unique<PDGraph>();

      combine(*Graph, FromGraph);
    }

    return *this;
  }

  PDGraphBuilder &setLazyConstruction(bool Mode) {
    LazyConstruction = Mode;

    return *this;
  }

  std::unique_ptr<PDGraph> build() {
    if (LazyConstruction && !componentGraphs.empty()) {
      Graph = std::make_unique<PDGraph>();

      for (const auto &e : componentGraphs)
        combine(*Graph, e);
    }

    return std::move(Graph);
  }
};

} // namespace pedigree end

#endif // header
