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
  PDGraphBuilder() = default;

  PDGraphBuilder &addGraph(InstructionDependenceGraph &Graph) {
    componentGraphs.emplace_back(std::cref(Graph));

    return *this;
  }

  std::unique_ptr<PDGraph> build() {
    if (!componentGraphs.empty()) {
      Graph = std::make_unique<PDGraph>();

      for (const auto &e : componentGraphs)
        combine(*Graph, e);
    }

    return std::move(Graph);
  }
};

} // namespace pedigree end

#endif // header
