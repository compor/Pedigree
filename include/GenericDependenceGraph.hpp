//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Dependence.hpp"

#include <utility>
// using std::pair

#include <vector>
// using std::vector

namespace pedigree {

template <typename NodeT> class GenericDependenceNode {
public:
  using NodeTy = NodeT;
  using UnderlyingTy = NodeT *;
  using DependenceRecordTy =
      std::pair<GenericDependenceNode *, DataDependenceInfo>;

private:
  using EdgeStorageTy = std::vector<DependenceRecordTy>;
  EdgeStorageTy m_Edges;

  inline void incrementDependeeCount() { ++m_DependeeCount; }
  inline void decrementDependeeCount() { --m_DependeeCount; }

  UnderlyingTy m_Underlying;
  unsigned m_DependeeCount;

public:
  using EdgesSizeTy = typename EdgeStorageTy::size_type;

  using iterator = typename EdgeStorageTy::iterator;
  using const_iterator = typename EdgeStorageTy::const_iterator;

  GenericDependenceNode(UnderlyingTy Unit)
      : m_Underlying(Unit), m_DependeeCount(0) {}

  UnderlyingTy getUnderlying() const { return m_Underlying; }

  void addDependentNode(GenericDependenceNode *Node) {
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

} // namespace pedigree end

#endif // header
