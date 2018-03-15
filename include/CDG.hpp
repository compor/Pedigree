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

#include <map>
// using std::map

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <algorithm>
// using std::for_each

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

using ControlDependenceNode = GenericDependenceNode<llvm::BasicBlock>;

class CDG {
  using NodeTy = ControlDependenceNode;
  using UnderlyingTy = NodeTy::UnderlyingTy;
  using NodeMapTy = std::map<UnderlyingTy, std::unique_ptr<NodeTy>>;
  NodeMapTy m_NodeMap;

public:
  using VerticesSizeTy = NodeMapTy::size_type;
  using EdgesSizeTy = NodeTy::EdgesSizeTy;
  // TODO hide the exposure of internal implemenation using smart pointer
  // TODO potentially use a std::reference_wrapper?
  // this is important because map-like containers expose the semantic
  // immutability of their keys as const key_type in their value_type
  using value_type = typename NodeMapTy::value_type;

  using iterator = NodeMapTy::iterator;
  using const_iterator = NodeMapTy::const_iterator;

  decltype(auto) getOrInsertNode(UnderlyingTy Unit) {
    auto &node = m_NodeMap[Unit];
    if (!node)
      node = std::make_unique<NodeTy>(Unit);

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

  const NodeTy *getEntryNode() const { return begin()->second.get(); }
  NodeTy *getEntryNode() { return begin()->second.get(); }
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

  // TODO we might need to hide the implementation detail of using a smart
  // pointer in the graph class by mapping the iterator provided by it in the
  // graph class itself
  // TODO this also forces us to use a ref to pass the node pair since the
  // current smart pointer used disallows copy
  static NodeType &NodeDeref(NodePairTy &P) { return *P.second.get(); }
  using NodeDerefFuncTy = std::function<decltype(NodeDeref)>;

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
};

} // namespace llvm end

#endif // header
