//
//
//

#ifndef PEDIGREE_DDG_HPP
#define PEDIGREE_DDG_HPP

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

#include <cstdint>
// using uint8_t

#include <cassert>
// using assert

namespace pedigree {

class DependenceNode {
public:
  using DependenceRecordTy =
      std::pair<DependenceNode *, DataDependenceInfo>;
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

  DependenceNode(UnderlyingTy Unit)
      : m_Underlying(Unit), m_DependeeCount(0) {}

  UnderlyingTy getUnderlying() const { return m_Underlying; }

  void addDependentNode(DependenceNode *Node) {
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

class DDG {
  using NodeMapTy = std::map<llvm::Instruction *, DependenceNode *>;
  NodeMapTy m_NodeMap;

public:
  using VerticesSizeTy = NodeMapTy::size_type;
  using EdgesSizeTy = DependenceNode::EdgesSizeTy;

  using iterator = NodeMapTy::iterator;
  using const_iterator = NodeMapTy::const_iterator;

  DDG() = default;
  ~DDG() {
    for (auto &e : m_NodeMap)
      delete e.second;
  }

  DependenceNode *
  getOrInsertNode(const llvm::Instruction *CurInstruction) {
    auto *curNode = const_cast<llvm::Instruction *>(CurInstruction);
    auto &node = m_NodeMap[curNode];
    if (node)
      return node;

    return node = new DependenceNode(curNode);
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

  const DependenceNode *getEntryNode() const { return begin()->second; }
  DependenceNode *getEntryNode() { return begin()->second; }
};

} // namespace pedigree end

namespace llvm {

// graph traits specializations

using namespace pedigree;

// this template specialization is meant to be used as a supplement to the main
// graph specialization
template <> struct GraphTraits<DependenceNode *> {
  using NodeType = DependenceNode;

  using ChildPairTy = DependenceNode::DependenceRecordTy;
  using ChildDerefFuncTy = std::function<DependenceNode *(ChildPairTy)>;

  using ChildIteratorType =
      llvm::mapped_iterator<NodeType::iterator, ChildDerefFuncTy>;

  static NodeType *getEntryNode(NodeType *G) { return G; }

  static ChildIteratorType child_begin(NodeType *G) {
    return llvm::map_iterator(G->begin(), ChildDerefFuncTy(ChildDeref));
  }
  static ChildIteratorType child_end(NodeType *G) {
    return llvm::map_iterator(G->end(), ChildDerefFuncTy(ChildDeref));
  }

  static DependenceNode *ChildDeref(ChildPairTy P) {
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
  }
};

template <>
struct GraphTraits<DDG *> : public GraphTraits<DependenceNode *> {
  using GraphTy = DDG;

  using NodePairTy = std::pair<llvm::Instruction *, DependenceNode *>;
  using NodeDerefFuncTy = std::function<DependenceNode &(NodePairTy)>;

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

  static DependenceNode &NodeDeref(NodePairTy P) { return *P.second; }
};

} // namespace llvm end

#endif // header
