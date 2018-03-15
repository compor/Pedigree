//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Dependence.hpp"

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include "boost/any.hpp"
// using boost::any

#include <vector>
// using std::vector

#include <utility>
// using std::pair

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

template <typename NodeT> class GenericDependenceNode {
public:
  using NodeTy = NodeT;
  using UnderlyingTy = NodeT *;

private:
  using DependenceRecordTy = std::pair<GenericDependenceNode *, boost::any>;
  using EdgeStorageTy = std::vector<DependenceRecordTy>;
  EdgeStorageTy m_Edges;

  inline void incrementDependeeCount() { ++m_DependeeCount; }
  inline void decrementDependeeCount() { --m_DependeeCount; }

  UnderlyingTy m_Underlying;
  unsigned m_DependeeCount;

public:
  using value_type = typename EdgeStorageTy::value_type;
  using EdgesSizeTy = typename EdgeStorageTy::size_type;

  using iterator = typename EdgeStorageTy::iterator;
  using const_iterator = typename EdgeStorageTy::const_iterator;

  GenericDependenceNode(UnderlyingTy Unit)
      : m_Underlying(Unit), m_DependeeCount(0) {}

  UnderlyingTy getUnderlying() const { return m_Underlying; }

  void addDependentNode(GenericDependenceNode *Node, const boost::any &Info) {
    m_Edges.emplace_back(Node, Info);
    Node->incrementDependeeCount();
  }

  EdgesSizeTy numEdges() const { return m_Edges.size(); }

  inline decltype(auto) begin() { return m_Edges.begin(); }
  inline decltype(auto) end() { return m_Edges.end(); }
  inline decltype(auto) begin() const { return m_Edges.begin(); }
  inline decltype(auto) end() const { return m_Edges.end(); }

  inline unsigned getDependeeCount() const { return m_DependeeCount; }
};

// graph traits specializations

// generic base for easing the task of creating graph traits for graph nodes

template <typename DependenceNodeT> struct DependenceNodeGraphTraitsBase {
  static_assert(
      std::is_same<typename std::is_pointer<DependenceNodeT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename DependenceNodeT>
struct DependenceNodeGraphTraitsBase<DependenceNodeT *> {
  using NodeType = DependenceNodeT;

  using ChildPairTy = typename NodeType::value_type;
  using ChildDerefFuncTy = std::function<NodeType *(ChildPairTy)>;

  using ChildIteratorType =
      llvm::mapped_iterator<typename NodeType::iterator, ChildDerefFuncTy>;

  static NodeType *getEntryNode(NodeType *G) { return G; }

  static ChildIteratorType child_begin(NodeType *G) {
    using std::begin;
    return llvm::map_iterator(begin(*G), ChildDerefFuncTy(ChildDeref));
  }

  static ChildIteratorType child_end(NodeType *G) {
    using std::end;
    return llvm::map_iterator(end(*G), ChildDerefFuncTy(ChildDeref));
  }

  static NodeType *ChildDeref(ChildPairTy P) {
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
  }
};

} // namespace pedigree end

#endif // header
