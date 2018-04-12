//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCENODE_HPP
#define PEDIGREE_GENERICDEPENDENCENODE_HPP

#include "Analysis/DependenceInfo/NoDependenceInfo.hpp"

#include "Support/Utils/TypeTraits.hpp"

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
// using std::pair

#include <functional>
// using std::function

namespace pedigree {

template <typename T, typename EdgeInfoT = NoDependenceInfo>
class GenericDependenceNode
    : boost::equality_comparable<GenericDependenceNode<T, EdgeInfoT>> {
public:
  using NodeType = GenericDependenceNode;
  using UnderlyingType = T *;
  using ConstUnderlyingType = const T *;
  using EdgeInfoType = EdgeInfoT;

private:
  using DependenceRecordType = std::pair<NodeType *, EdgeInfoType>;
  using EdgeStorageType = std::vector<DependenceRecordType>;

  EdgeStorageType Edges;
  UnderlyingType Underlying;
  mutable unsigned DependeeCount;

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
      : Underlying(Under),
        DependeeCount(0) {}

  GenericDependenceNode(const GenericDependenceNode &) = delete;
  GenericDependenceNode &operator=(const GenericDependenceNode &) = delete;

  GenericDependenceNode(GenericDependenceNode &&Other) noexcept(
      are_all_nothrow_move_constructible<decltype(Edges), decltype(Underlying),
                                         decltype(DependeeCount)>::value)
      : Edges(std::move(Other.Edges)), Underlying(std::move(Other.Underlying)),
        DependeeCount(std::move(Other.DependeeCount)) {
    Other.Edges.clear();
    Other.Underlying = {};
    Other.DependeeCount = {};
  }

  GenericDependenceNode &operator=(GenericDependenceNode &&Other) noexcept(
      are_all_nothrow_move_assignable<decltype(Edges), decltype(Underlying),
                                      decltype(DependeeCount)>::value) {
    Edges = std::move(Other.Edges);
    Underlying = std::move(Other.Underlying);
    DependeeCount = std::move(Other.DependeeCount);

    Other.Edges.clear();
    Other.Underlying = {};
    Other.DependeeCount = {};

    return *this;
  };

  UnderlyingType get() const noexcept { return Underlying; }

  bool hasEdgeWith(const NodeType *Node) const {
    return Edges.end() != getEdgeWith(Node);
  }

  void addDependentNode(const NodeType *Node, EdgeInfoType Info) {
    Edges.emplace_back(const_cast<NodeType *>(Node), std::move(Info));
    Node->incrementDependeeCount();
  }

  boost::optional<const EdgeInfoType &>
  getEdgeInfo(const NodeType *Node) const {
    auto found = getEdgeWith(Node);

    return found != Edges.end()
               ? boost::optional<const EdgeInfoType &>((*found).second)
               : boost::none;
  }

  bool setEdgeInfo(const NodeType *Node, EdgeInfoType Info) {
    auto found = getEdgeWith(Node);

    if (found == Edges.end()) {
      return false;
    }

    (*found).second = std::move(Info);

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
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
  }

  static const NodeType *nodes_const_iterator_map(const value_type &P) {
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
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
      otherChildren.insert(e.first->get());

    for (const auto &e : *this)
      if (otherChildren.count(e.first->get()) == 0) {
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
                        [&Node](const auto &e) { return Node == e.first; });
  }

  iterator getEdgeWith(const NodeType *Node) {
    return std::find_if(Edges.begin(), Edges.end(),
                        [&Node](const auto &e) { return Node == e.first; });
  }
};

} // namespace pedigree

#endif // header
