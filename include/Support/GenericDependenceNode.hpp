//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCENODE_HPP
#define PEDIGREE_GENERICDEPENDENCENODE_HPP

#include "Analysis/DependenceInfo/NoDependenceInfo.hpp"

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

  void incrementDependeeCount() { ++DependeeCount; }
  void decrementDependeeCount() { --DependeeCount; }

  EdgeStorageType Edges;
  UnderlyingType Underlying;
  unsigned DependeeCount;

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

  explicit GenericDependenceNode(UnderlyingType Under)
      : Underlying(Under), DependeeCount(0) {}

  GenericDependenceNode(const GenericDependenceNode &) = delete;
  GenericDependenceNode &operator=(const GenericDependenceNode &) = delete;

  GenericDependenceNode(GenericDependenceNode &&Other)
      : Edges(std::move(Other.Edges)), Underlying(std::move(Other.Underlying)),
        DependeeCount(std::move(Other.DependeeCount)) {
    Other.Edges.clear();
    Other.Underlying = {};
    Other.DependeeCount = {};
  }

  GenericDependenceNode &operator=(GenericDependenceNode &&Other) {
    Edges = std::move(Other.Edges);
    Underlying = std::move(Other.Underlying);
    DependeeCount = std::move(Other.DependeeCount);

    Other.Edges.clear();
    Other.Underlying = {};
    Other.DependeeCount = {};
    return *this;
  };

  UnderlyingType get() const { return Underlying; }

  void addDependentNode(NodeType *Node, const EdgeInfoType &Info) {
    Edges.emplace_back(Node, Info);
    Node->incrementDependeeCount();
  }

  boost::optional<const EdgeInfoType &> getEdgeInfo(NodeType *Node) const {
    auto found =
        std::find_if(Edges.begin(), Edges.end(),
                     [&Node](const auto &e) { return Node == e.first; });

    return found != Edges.end()
               ? boost::optional<const EdgeInfoType &>((*found).second)
               : boost::none;
  }

  bool setEdgeInfo(const NodeType *Node, const EdgeInfoType &Info) const {
    return true;
  }

  EdgesSizeType numEdges() const { return Edges.size(); }
  decltype(auto) size() const { return numEdges(); }

  decltype(auto) begin() { return Edges.begin(); }
  decltype(auto) begin() const { return Edges.begin(); }
  decltype(auto) end() { return Edges.end(); }
  decltype(auto) end() const { return Edges.end(); }

  decltype(auto) edges_begin() { return begin(); }
  decltype(auto) edges_begin() const { return begin(); }
  decltype(auto) edges_end() { return end(); }
  decltype(auto) edges_end() const { return end(); }

  decltype(auto) edges() {
    return llvm::make_range(edges_begin(), edges_end());
  }

  decltype(auto) edges() const {
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

  decltype(auto) nodes_begin() {
    return nodes_iterator(Edges.begin(), nodes_iterator_map);
  }

  decltype(auto) nodes_begin() const {
    return const_nodes_iterator(Edges.begin(), nodes_const_iterator_map);
  }

  decltype(auto) nodes_end() {
    return nodes_iterator(Edges.end(), nodes_iterator_map);
  }

  decltype(auto) nodes_end() const {
    return const_nodes_iterator(Edges.end(), nodes_const_iterator_map);
  }

  decltype(auto) nodes() {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  decltype(auto) nodes() const {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  unsigned getDependeeCount() const { return DependeeCount; }

  bool compare(const GenericDependenceNode &Other) const {
    if (this->numEdges() != Other.numEdges())
      return true;

    llvm::SmallPtrSet<ConstUnderlyingType, 8> otherChildren;
    for (const auto &e : Other)
      otherChildren.insert(e.first->get());

    for (const auto &e : *this)
      if (otherChildren.count(e.first->get()) == 0)
        return true;

    return false;
  }

  bool operator==(const GenericDependenceNode &Other) const {
    return !compare(Other);
  }
};

} // namespace pedigree end

#endif // header
