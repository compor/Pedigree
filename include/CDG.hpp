//
//
//

#ifndef PEDIGREE_CDG_HPP
#define PEDIGREE_CDG_HPP

#include "Dependence.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <vector>
// using std::vector

#include <utility>
// using std::pair

namespace pedigree {

class ControlDependenceNode {
public:
  using DependenceRecordTy =
      std::pair<ControlDependenceNode *, DataDependenceInfo>;
  using UnderlyingTy = llvm::BasicBlock *;

private:
  using EdgeStorageTy = std::vector<DependenceRecordTy>;
  EdgeStorageTy m_Edges;

  inline void incrementDependeeCount() { ++m_DependeeCount; }
  inline void decrementDependeeCount() { --m_DependeeCount; }

  llvm::BasicBlock *m_Underlying;
  unsigned m_DependeeCount;

public:
  using EdgesSizeTy = EdgeStorageTy::size_type;

  using iterator = EdgeStorageTy::iterator;
  using const_iterator = EdgeStorageTy::const_iterator;

  ControlDependenceNode(llvm::BasicBlock *Unit)
      : m_Underlying(Unit), m_DependeeCount(0) {}

  llvm::BasicBlock *getUnderlying() const { return m_Underlying; }

  void addDependentNode(ControlDependenceNode *Node) {
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

#endif // header
