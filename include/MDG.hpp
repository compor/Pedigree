//
//
//

#ifndef PEDIGREE_MDG_HPP
#define PEDIGREE_MDG_HPP

#include "Dependence.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <vector>
// using std::vector

#include <map>
// using std::map

#include <utility>
// using std::pair

#include <algorithm>
// using std::for_each

#include <cassert>
// using assert

namespace pedigree {

class MemoryDependenceNode {
public:
  using DependenceRecordTy =
      std::pair<MemoryDependenceNode *, DataDependenceInfo>;
  using UnderlyingTy = llvm::Instruction *;

private:
  using EdgeStorageTy = std::vector<DependenceRecordTy>;
  EdgeStorageTy m_Edges;

  inline void incrementDependeeCount() { ++m_DependeeCount; }
  inline void decrementDependeeCount() { --m_DependeeCount; }

  UnderlyingTy m_Underlying;
  unsigned m_DependeeCount;

public:
  using EdgesSizeTy = EdgeStorageTy::size_type;

  using iterator = EdgeStorageTy::iterator;
  using const_iterator = EdgeStorageTy::const_iterator;

  MemoryDependenceNode(UnderlyingTy Unit)
      : m_Underlying(Unit), m_DependeeCount(0) {}

  UnderlyingTy getUnderlying() const { return m_Underlying; }

  void addDependentNode(MemoryDependenceNode *Node) {
    m_Edges.emplace_back(
        Node, DataDependenceInfo{DependenceType::flow, DependenceOrigin::data});

    Node->incrementDependeeCount();
  }

  EdgesSizeTy numEdges() const { return m_Edges.size(); }

  inline decltype(auto) begin() { return m_Edges.begin(); }
  inline decltype(auto) end() { return m_Edges.end(); }
  inline decltype(auto) begin() const { return m_Edges.begin(); }
  inline decltype(auto) end() const { return m_Edges.end(); }

  inline unsigned getDependeeCount() const { return m_DependeeCount; }
};

class MDG {
  using UnderlyingTy = MemoryDependenceNode::UnderlyingTy;
  using NodeMapTy = std::map<UnderlyingTy, MemoryDependenceNode *>;
  NodeMapTy m_NodeMap;

public:
  using VerticesSizeTy = NodeMapTy::size_type;
  using EdgesSizeTy = MemoryDependenceNode::EdgesSizeTy;

  using iterator = NodeMapTy::iterator;
  using const_iterator = NodeMapTy::const_iterator;

  MDG() = default;
  ~MDG() {
    for (auto &e : m_NodeMap)
      delete e.second;
  }

  MemoryDependenceNode *getOrInsertNode(UnderlyingTy Unit) {
    auto &node = m_NodeMap[Unit];
    if (node)
      return node;

    return node = new MemoryDependenceNode(Unit);
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

  const MemoryDependenceNode *getEntryNode() const { return begin()->second; }
  MemoryDependenceNode *getEntryNode() { return begin()->second; }
};

} // namespace pedigree end

namespace llvm {

// graph traits specializations

using namespace pedigree;

// this template specialization is meant to be used as a supplement to the main
// graph specialization
template <> struct GraphTraits<MemoryDependenceNode *> {
  using NodeType = MemoryDependenceNode;

  using ChildPairTy = MemoryDependenceNode::DependenceRecordTy;
  using ChildDerefFuncTy = std::function<MemoryDependenceNode *(ChildPairTy)>;

  using ChildIteratorType =
      llvm::mapped_iterator<NodeType::iterator, ChildDerefFuncTy>;

  static NodeType *getEntryNode(NodeType *G) { return G; }

  static ChildIteratorType child_begin(NodeType *G) {
    return llvm::map_iterator(G->begin(), ChildDerefFuncTy(ChildDeref));
  }
  static ChildIteratorType child_end(NodeType *G) {
    return llvm::map_iterator(G->end(), ChildDerefFuncTy(ChildDeref));
  }

  static MemoryDependenceNode *ChildDeref(ChildPairTy P) {
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
  }
};

template <>
struct GraphTraits<MDG *> : public GraphTraits<MemoryDependenceNode *> {
  using GraphTy = MDG;

  using NodePairTy = std::pair<llvm::Instruction *, MemoryDependenceNode *>;
  using NodeDerefFuncTy = std::function<MemoryDependenceNode &(NodePairTy)>;

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

  static MemoryDependenceNode &NodeDeref(NodePairTy P) { return *P.second; }
};

} // namespace llvm end

#endif // header
