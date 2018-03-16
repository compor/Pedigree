//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Dependence.hpp"

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <vector>
// using std::vector

#include <map>
// using std::map

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <algorithm>
// using std::for_each

#include <utility>
// using std::pair

#include <type_traits>
// using std::is_same
// using std::is_pointer
// using std::false_type

#include <functional>
// using std::function

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

struct NoEdgeInfo {};

template <typename WrappedNodeT, typename EdgeInfoT = NoEdgeInfo>
class GenericDependenceNode {
public:
  using NodeType = GenericDependenceNode;
  using UnderlyingTy = WrappedNodeT *;
  using EdgeInfoTy = EdgeInfoT;

private:
  using DependenceRecordTy = std::pair<GenericDependenceNode *, EdgeInfoTy>;
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

  void addDependentNode(GenericDependenceNode *Node, const EdgeInfoTy &Info) {
    m_Edges.emplace_back(Node, Info);
    Node->incrementDependeeCount();
  }

  EdgesSizeTy numEdges() const { return m_Edges.size(); }
  decltype(auto) size() const { return numEdges(); }

  inline decltype(auto) begin() { return m_Edges.begin(); }
  inline decltype(auto) end() { return m_Edges.end(); }
  inline decltype(auto) begin() const { return m_Edges.begin(); }
  inline decltype(auto) end() const { return m_Edges.end(); }

  static GenericDependenceNode *nodes_iterator_map(value_type &P) {
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
  }

  using nodes_iterator = llvm::mapped_iterator<
      iterator, std::function<GenericDependenceNode *(value_type &)>>;

  inline decltype(auto) nodes_begin() {
    return nodes_iterator(m_Edges.begin(), nodes_iterator_map);
  }

  inline decltype(auto) nodes_end() {
    return nodes_iterator(m_Edges.end(), nodes_iterator_map);
  }

  inline unsigned getDependeeCount() const { return m_DependeeCount; }
};

template <typename NodeT> class GenericDependenceGraph {
  using NodeTy = NodeT;
  using UnderlyingTy = typename NodeTy::UnderlyingTy;
  using NodeMapTy = std::map<UnderlyingTy, std::unique_ptr<NodeTy>>;
  NodeMapTy m_NodeMap;

public:
  using VerticesSizeTy = typename NodeMapTy::size_type;
  using EdgesSizeTy = typename NodeTy::EdgesSizeTy;
  // TODO hide the exposure of internal implemenation using smart pointer
  // TODO potentially use a std::reference_wrapper?
  // this is important because map-like containers expose the semantic
  // immutability of their keys as const key_type in their value_type
  using value_type = typename NodeMapTy::value_type;

  using iterator = typename NodeMapTy::iterator;
  using const_iterator = typename NodeMapTy::const_iterator;

  decltype(auto) getOrInsertNode(UnderlyingTy Unit) {
    auto &node = m_NodeMap[Unit];
    if (!node)
      node = std::make_unique<NodeTy>(Unit);

    return node.get();
  }

  VerticesSizeTy numVertices() const { return m_NodeMap.size(); }

  EdgesSizeTy numEdges() const {
    EdgesSizeTy n{};
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

  static NodeType *getEntryNode(NodeType *G) { return G; }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename DependenceNodeT::nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  using nodes_iterator = typename DependenceNodeT::nodes_iterator;
  static decltype(auto) nodes_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeType *G) { return G->nodes_end(); }
};

} // namespace pedigree end

#endif // header
