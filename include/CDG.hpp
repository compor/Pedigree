//
//
//

#ifndef PEDIGREE_CDG_HPP
#define PEDIGREE_CDG_HPP

#include "Dependence.hpp"

#include "GenericDependenceGraph.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <vector>
// using std::vector

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <utility>
// using std::pair

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

using ControlDependenceNode = GenericDependenceNode<llvm::BasicBlock>;

class CDG {
  using UnderlyingTy = ControlDependenceNode::UnderlyingTy;
  using NodeMapTy =
      std::map<UnderlyingTy, std::unique_ptr<ControlDependenceNode>>;
  NodeMapTy m_NodeMap;

public:
  using VerticesSizeTy = NodeMapTy::size_type;
  using EdgesSizeTy = ControlDependenceNode::EdgesSizeTy;
  using value_type = typename NodeMapTy::value_type;

  using iterator = NodeMapTy::iterator;
  using const_iterator = NodeMapTy::const_iterator;

  CDG() = default;
  ~CDG() = default;

  decltype(auto) getOrInsertNode(UnderlyingTy Unit) {
    auto &node = m_NodeMap[Unit];
    if (!node)
      node = std::make_unique<ControlDependenceNode>(Unit);

    return node.get();
  }

  VerticesSizeTy numVertices() const { return m_NodeMap.size(); }

  EdgesSizeTy numEdges() const {
    NodeMapTy::size_type n{};
    std::for_each(std::begin(m_NodeMap), std::end(m_NodeMap),
                  [&n](const auto &e) { n += e.second.get()->numEdges(); });
    return n;
  }

  inline decltype(auto) begin() { return m_NodeMap.begin(); }
  inline decltype(auto) end() { return m_NodeMap.end(); }
  inline decltype(auto) begin() const { return m_NodeMap.begin(); }
  inline decltype(auto) end() const { return m_NodeMap.end(); }

  const ControlDependenceNode *getEntryNode() const {
    return begin()->second.get();
  }
  ControlDependenceNode *getEntryNode() { return begin()->second.get(); }
};

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::ControlDependenceNode *>
    : public pedigree::DependenceNodeGraphTraitsBase<
          pedigree::ControlDependenceNode *> {};

template <>
struct GraphTraits<pedigree::CDG *>
    : public GraphTraits<pedigree::ControlDependenceNode *> {
  using GraphTy = pedigree::CDG;

  using NodePairTy = GraphTy::value_type;
  using NodeDerefFuncTy =
      std::function<pedigree::ControlDependenceNode &(NodePairTy &)>;

  using nodes_iterator =
      llvm::mapped_iterator<GraphTy::iterator, NodeDerefFuncTy>;

  static NodeType *getEntryNode(GraphTy *G) { return G->getEntryNode(); }

  static nodes_iterator nodes_begin(GraphTy *G) {
    using std::begin;
    return llvm::map_iterator(begin(*G), NodeDerefFuncTy(NodeDeref));
  }
  static nodes_iterator nodes_end(GraphTy *G) {
    using std::end;
    return llvm::map_iterator(end(*G), NodeDerefFuncTy(NodeDeref));
  }

  static unsigned size(GraphTy *G) {
    return static_cast<unsigned>(G->numVertices());
  }

  static pedigree::ControlDependenceNode &NodeDeref(NodePairTy &P) {
    return *P.second.get();
  }
};

} // namespace llvm end

#endif // header
