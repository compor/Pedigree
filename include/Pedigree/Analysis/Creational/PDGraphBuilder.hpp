//
//
//

#ifndef PEDIGREE_PDGRAPHBUILDER_HPP
#define PEDIGREE_PDGRAPHBUILDER_HPP

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include "Pedigree/Analysis/Graphs/PDGraph.hpp"

#include "Pedigree/Exchange/MetadataReader.hpp"

#include <vector>
// using std::vector

#include <functional>
// using std::cref
// using std::function

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <cassert>
// using assert

namespace pedigree {

class PDGraphBuilder {
  using NodeType = InstructionDependenceGraph::NodeType;

public:
  using PostInsertionFunc = std::function<void(
      NodeType::NodeInfoType::value_type &, const NodeType::UnitType)>;

private:
  std::vector<std::reference_wrapper<const InstructionDependenceGraph>>
      componentGraphs;
  std::unique_ptr<PDGraph> Graph;
  bool LazilyConstructible;

  std::vector<PostInsertionFunc> PostInsertionFunctions;

  void postInsertionProcess(NodeType *N) {
    for (auto f : PostInsertionFunctions) {
      f(N->info(), N->unit());
    }
  }

  // TODO consider moving this to the graph, maybe as an operator
  void combine(InstructionDependenceGraph &ToGraph,
               const InstructionDependenceGraph &FromGraph) {
    assert(Graph && "Graph is null!");

    using GT = llvm::GraphTraits<decltype(&FromGraph)>;

    for (const auto &node : GT::nodes(&FromGraph)) {
      auto src = ToGraph.getOrInsertNode(node->unit());
      postInsertionProcess(src);

      for (const auto &child : GT::children(node)) {
        auto dst = ToGraph.getOrInsertNode(child->unit());
        postInsertionProcess(dst);

        auto newInfo = node->getEdgeInfo(child);

        if (!src->hasEdgeWith(dst)) {
          src->addDependentNode(dst, newInfo.value());
        } else {
          auto curInfo = src->getEdgeInfo(dst);
          src->setEdgeInfo(dst, curInfo.value() | newInfo.value());
        }
      }
    }
  }

public:
  PDGraphBuilder() : LazilyConstructible(false) {}

  PDGraphBuilder &addGraph(const InstructionDependenceGraph &FromGraph) {
    if (LazilyConstructible) {
      componentGraphs.emplace_back(std::cref(FromGraph));
    } else {
      if (!Graph) {
        Graph = std::make_unique<PDGraph>();
      }

      combine(*Graph, FromGraph);
    }

    return *this;
  }

  PDGraphBuilder &setLazilyConstructible(bool Mode) {
    LazilyConstructible = Mode;

    return *this;
  }

  PDGraphBuilder &registerPostInsertionCallback(const PostInsertionFunc &Func) {
    PostInsertionFunctions.emplace_back(Func);

    return *this;
  }

  std::unique_ptr<PDGraph> build() {
    if (!Graph) {
      Graph = std::make_unique<PDGraph>();
    }

    if (LazilyConstructible && !componentGraphs.empty()) {
      for (const auto &e : componentGraphs) {
        combine(*Graph, e);
      }
    }

    return std::move(Graph);
  }
};

} // namespace pedigree

#endif // header
