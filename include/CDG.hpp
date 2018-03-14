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

#include <utility>
// using std::pair

namespace pedigree {

using ControlDependenceNode = GenericDependenceNode<llvm::BasicBlock>;

class CDG {
  using UnderlyingTy = ControlDependenceNode::UnderlyingTy;
  using NodeMapTy = std::map<UnderlyingTy, ControlDependenceNode *>;
  NodeMapTy m_NodeMap;

public:
  using VerticesSizeTy = NodeMapTy::size_type;
  using EdgesSizeTy = ControlDependenceNode::EdgesSizeTy;

  using iterator = NodeMapTy::iterator;
  using const_iterator = NodeMapTy::const_iterator;

  CDG() = default;
  ~CDG() {
    for (auto &e : m_NodeMap)
      delete e.second;
  }

  ControlDependenceNode *getOrInsertNode(UnderlyingTy Unit) {
    auto &node = m_NodeMap[Unit];
    if (node)
      return node;

    return node = new ControlDependenceNode(Unit);
  }

  VerticesSizeTy numVertices() const { return m_NodeMap.size(); }

  EdgesSizeTy numEdges() const {
    NodeMapTy::size_type n{};
    std::for_each(std::begin(m_NodeMap), std::end(m_NodeMap),
                  [&n](const auto &e) { n += e.second->numEdges(); });
    return n;
  }

  inline decltype(auto) begin() { return m_NodeMap.begin(); }
  inline decltype(auto) end() { return m_NodeMap.end(); }
  inline decltype(auto) begin() const { return m_NodeMap.begin(); }
  inline decltype(auto) end() const { return m_NodeMap.end(); }

  const ControlDependenceNode *getEntryNode() const { return begin()->second; }
  ControlDependenceNode *getEntryNode() { return begin()->second; }
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

  using NodePairTy =
      std::pair<llvm::BasicBlock *, pedigree::ControlDependenceNode *>;
  using NodeDerefFuncTy =
      std::function<pedigree::ControlDependenceNode &(NodePairTy)>;

  using nodes_iterator =
      llvm::mapped_iterator<GraphTy::iterator, NodeDerefFuncTy>;

  static NodeType *getEntryNode(GraphTy *G) { return G->getEntryNode(); }

  static nodes_iterator nodes_begin(GraphTy *G) {
    return llvm::map_iterator(G->begin(), NodeDerefFuncTy(NodeDeref));
  }
  static nodes_iterator nodes_end(GraphTy *G) {
    return llvm::map_iterator(G->end(), NodeDerefFuncTy(NodeDeref));
  }

  static unsigned size(GraphTy *G) {
    return static_cast<unsigned>(G->numVertices());
  }

  static pedigree::ControlDependenceNode &NodeDeref(NodePairTy P) {
    return *P.second;
  }
};

} // namespace llvm end

#endif // header
