//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCENODE_HPP
#define PEDIGREE_GENERICDEPENDENCENODE_HPP

#include "Support/Traits/TypeTraits.hpp"

#include "llvm/ADT/SmallPtrSet.h"
// using llvm::SmallPtrSet

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

#include "boost/optional.hpp"
// using boost::optional

#include "boost/operators.hpp"
// using boost::equality_comparable

#include <vector>
// using std::vector

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <utility>
// using std::move

#include <functional>
// using std::function

#include <type_traits>
// using std::enable_if
// using std::is_void

namespace pedigree {

namespace detail {

template <typename T, typename U> struct EdgeRecordImpl {
  using node_type = T;
  using data_type = U;

  T node;
  U data;
};

template <typename T> struct EdgeRecordImpl<T, void> {
  using node_type = T;

  T node;
};

//

template <typename U> struct NodeInfoImpl { U data; };
template <> struct NodeInfoImpl<void> {};

} // namespace detail

template <typename T, typename NodeInfoT = void, typename EdgeInfoT = void>
class GenericDependenceNode
    : boost::equality_comparable<GenericDependenceNode<T, EdgeInfoT>> {
public:
  using NodeType = GenericDependenceNode;
  using UnderlyingType = T *;
  using ConstUnderlyingType = const T *;
  using NodeInfoType = NodeInfoT;
  using EdgeInfoType = EdgeInfoT;

private:
  using DependenceRecordType = detail::EdgeRecordImpl<NodeType *, EdgeInfoType>;
  using EdgeStorageType = std::vector<DependenceRecordType>;

  mutable unsigned DependeeCount;
  UnderlyingType Underlying;
  detail::NodeInfoImpl<NodeInfoType> NodeInfo;
  EdgeStorageType Edges;

public:
  using value_type = typename EdgeStorageType::value_type;
  using EdgesSizeType = typename EdgeStorageType::size_type;

  using iterator = typename EdgeStorageType::iterator;
  using const_iterator = typename EdgeStorageType::const_iterator;

  using nodes_iterator =
      llvm::mapped_iterator<iterator, std::function<NodeType *(value_type &)>>;

  using const_nodes_iterator = llvm::mapped_iterator<
      const_iterator, std::function<const NodeType *(const value_type &)>>;

  using edges_iterator = iterator;
  using const_edges_iterator = const_iterator;

  explicit GenericDependenceNode(UnderlyingType Under) noexcept
      : DependeeCount(0),
        Underlying(Under) {}

  GenericDependenceNode(const GenericDependenceNode &) = delete;
  GenericDependenceNode &operator=(const GenericDependenceNode &) = delete;

  GenericDependenceNode(GenericDependenceNode &&Other) noexcept(
      are_all_nothrow_move_constructible_v<decltype(DependeeCount),
                                           decltype(Underlying),
                                           decltype(NodeInfo), decltype(Edges)>)
      : DependeeCount(std::move(Other.DependeeCount)),
        Underlying(std::move(Other.Underlying)),
        NodeInfo(std::move(Other.NodeInfo)), Edges(std::move(Other.Edges)) {
    Other.DependeeCount = {};
    Other.Underlying = {};
    Other.NodeInfo = {};
    Other.Edges.clear();
  }

  GenericDependenceNode &operator=(GenericDependenceNode &&Other) noexcept(
      are_all_nothrow_move_assignable_v<decltype(DependeeCount),
                                        decltype(Underlying),
                                        decltype(NodeInfo), decltype(Edges)>) {
    DependeeCount = std::move(Other.DependeeCount);
    Underlying = std::move(Other.Underlying);
    NodeInfo = std::move(Other.NodeInfo);
    Edges = std::move(Other.Edges);

    Other.DependeeCount = {};
    Other.Underlying = {};
    Other.NodeInfo = {};
    Other.Edges.clear();

    return *this;
  };

  UnderlyingType get() const noexcept { return Underlying; }

  bool hasEdgeWith(const NodeType *Node) const {
    return Edges.end() != getEdgeWith(Node);
  }

  template <typename U = NodeInfoType>
  std::enable_if_t<!std::is_void<U>::value, bool> setNodeInfo(U Info) {
    NodeInfo.data = std::move(Info);
    return true;
  }

  template <typename U = NodeInfoType>
  std::enable_if_t<!std::is_void<U>::value, U> getNodeInfo() const {
    return NodeInfo.data;
  }

  template <typename U = EdgeInfoType>
  std::enable_if_t<!std::is_void<U>::value>
  addDependentNode(const NodeType *Node, U Info) {
    Edges.emplace_back(detail::EdgeRecordImpl<NodeType *, U>{
        const_cast<NodeType *>(Node), std::move(Info)});
    Node->incrementDependeeCount();
  }

  template <typename U = EdgeInfoType>
  std::enable_if_t<std::is_void<U>::value>
  addDependentNode(const NodeType *Node) {
    Edges.emplace_back(
        detail::EdgeRecordImpl<NodeType *, U>{const_cast<NodeType *>(Node)});
    Node->incrementDependeeCount();
  }

  template <typename U = EdgeInfoType>
  std::enable_if_t<!std::is_void<U>::value, boost::optional<const U &>>
  getEdgeInfo(const NodeType *Node) const {
    auto found = getEdgeWith(Node);

    return found != Edges.end() ? boost::optional<const U &>((*found).data)
                                : boost::none;
  }

  template <typename U = EdgeInfoType>
  std::enable_if_t<!std::is_void<U>::value, bool>
  setEdgeInfo(const NodeType *Node, U Info) {
    auto found = getEdgeWith(Node);

    if (found == Edges.end()) {
      return false;
    }

    (*found).data = std::move(Info);

    return true;
  }

  EdgesSizeType numEdges() const noexcept(noexcept(Edges.size())) {
    return Edges.size();
  }

  decltype(auto) size() const noexcept(noexcept(numEdges())) {
    return numEdges();
  }

  decltype(auto) begin() noexcept(noexcept(Edges.begin())) {
    return Edges.begin();
  }

  decltype(auto) begin() const noexcept(noexcept(Edges.begin())) {
    return Edges.begin();
  }

  decltype(auto) end() noexcept(noexcept(Edges.end())) { return Edges.end(); }

  decltype(auto) end() const noexcept(noexcept(Edges.end())) {
    return Edges.end();
  }

  decltype(auto) edges_begin() noexcept(noexcept(begin())) { return begin(); }

  decltype(auto) edges_begin() const noexcept(noexcept(begin())) {
    return begin();
  }

  decltype(auto) edges_end() noexcept(noexcept(end())) { return end(); }
  decltype(auto) edges_end() const noexcept(noexcept(end())) { return end(); }

  decltype(auto)
  edges() noexcept(noexcept(llvm::make_range(edges_begin(), edges_end()))) {
    return llvm::make_range(edges_begin(), edges_end());
  }

  decltype(auto) edges() const
      noexcept(noexcept(llvm::make_range(edges_begin(), edges_end()))) {
    return llvm::make_range(edges_begin(), edges_end());
  }

  static NodeType *nodes_iterator_map(value_type &P) {
    assert(P.node && "Pointer to graph node is null!");
    return P.node;
  }

  static const NodeType *nodes_const_iterator_map(const value_type &P) {
    assert(P.node && "Pointer to graph node is null!");
    return P.node;
  }

  decltype(auto)
  nodes_begin() noexcept(noexcept(nodes_iterator(Edges.begin(), {}))) {
    return nodes_iterator(Edges.begin(), nodes_iterator_map);
  }

  decltype(auto) nodes_begin() const
      noexcept(noexcept(const_nodes_iterator(Edges.begin(), {}))) {
    return const_nodes_iterator(Edges.begin(), nodes_const_iterator_map);
  }

  decltype(auto)
  nodes_end() noexcept(noexcept(nodes_iterator(Edges.end(), {}))) {
    return nodes_iterator(Edges.end(), nodes_iterator_map);
  }

  decltype(auto) nodes_end() const
      noexcept(noexcept(const_nodes_iterator(Edges.end(), {}))) {
    return const_nodes_iterator(Edges.end(), nodes_const_iterator_map);
  }

  decltype(auto)
  nodes() noexcept(noexcept(llvm::make_range(nodes_begin(), nodes_end()))) {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  decltype(auto) nodes() const
      noexcept(noexcept(llvm::make_range(nodes_begin(), nodes_end()))) {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  unsigned getDependeeCount() const noexcept { return DependeeCount; }

  bool compare(const GenericDependenceNode &Other) const {
    if (this->numEdges() != Other.numEdges()) {
      return true;
    }

    llvm::SmallPtrSet<ConstUnderlyingType, 8> otherChildren;
    for (const auto &e : Other)
      otherChildren.insert(e.node->get());

    for (const auto &e : *this)
      if (otherChildren.count(e.node->get()) == 0) {
        return true;
      }

    return false;
  }

  bool operator==(const GenericDependenceNode &Other) const {
    return !compare(Other);
  }

private:
  void incrementDependeeCount() const noexcept { ++DependeeCount; }
  void decrementDependeeCount() const noexcept { --DependeeCount; }

  const_iterator getEdgeWith(const NodeType *Node) const {
    return std::find_if(Edges.begin(), Edges.end(),
                        [&Node](const auto &e) { return Node == e.node; });
  }

  iterator getEdgeWith(const NodeType *Node) {
    return std::find_if(Edges.begin(), Edges.end(),
                        [&Node](const auto &e) { return Node == e.node; });
  }
};

} // namespace pedigree

#endif // header
