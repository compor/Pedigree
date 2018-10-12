//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Config.hpp"

#include "GenericDependenceGraphEdgeIterator.hpp"

#include "Support/Traits/TypeTraits.hpp"

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

#include "boost/optional.hpp"
// using boost::optional
// using boost::none
// using boost::make_optional

#include "boost/operators.hpp"
// using boost::equality_comparable

#include <map>
// using std::map

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <algorithm>
// using std::for_each

#include <functional>
// using std::function

#include <iterator>
// using std::begin
// using std::end

#include <utility>
// using std::declval

namespace pedigree {

template <typename NodeT>
class GenericDependenceGraph
    : boost::equality_comparable<GenericDependenceGraph<NodeT>> {
public:
  using NodeType = NodeT;

private:
  using UnitType = typename NodeType::UnitType;
  using NodeMapType = std::map<UnitType, std::unique_ptr<NodeType>>;

  NodeMapType NodeMap;
  NodeType *ExternalRoot;

public:
  using VerticesSizeType = typename NodeMapType::size_type;
  using EdgesSizeType = typename NodeType::EdgesSizeType;
  // TODO hide the exposure of internal implemenation using smart pointer
  // TODO potentially use a std::reference_wrapper?
  // this is important because map-like containers expose the semantic
  // immutability of their keys as const key_type in their value_type
  using value_type = typename NodeMapType::value_type;

  using iterator = typename NodeMapType::iterator;
  using const_iterator = typename NodeMapType::const_iterator;

  using nodes_iterator =
      llvm::mapped_iterator<iterator, std::function<NodeType *(value_type &)>>;

  using const_nodes_iterator = llvm::mapped_iterator<
      const_iterator, std::function<const NodeType *(const value_type &)>>;

  using edges_iterator = detail::GenericDependenceGraphEdgeIterator<
      nodes_iterator, typename NodeType::edges_iterator>;

  using const_edges_iterator = detail::GenericDependenceGraphEdgeIterator<
      const_nodes_iterator, typename NodeType::const_edges_iterator>;

  // explicitly define default methods
  // workaround for gcc bug 57728 which is present in clang too
  // GenericDependenceGraph() = default;
  GenericDependenceGraph() noexcept
      : ExternalRoot(getOrInsertNode(UnitType())){};

  GenericDependenceGraph(const GenericDependenceGraph &) = delete;
  GenericDependenceGraph &operator=(const GenericDependenceGraph &) = delete;

  explicit GenericDependenceGraph(GenericDependenceGraph &&Other) noexcept(
      noexcept(Other.NodeMap.clear()) &&
      are_all_nothrow_move_constructible_v<decltype(ExternalRoot),
                                           decltype(NodeMap)>)
      : ExternalRoot(std::move(Other.ExternalRoot)),
        NodeMap(std::move(Other.NodeMap)) {
    Other.NodeMap.clear();
  }

  GenericDependenceGraph &operator=(GenericDependenceGraph &&Other) noexcept(
      noexcept(Other.NodeMap.clear()) &&
      are_all_nothrow_move_assignable_v<decltype(ExternalRoot),
                                        decltype(NodeMap)>) {
    NodeMap = std::move(Other.NodeMap);
    Other.NodeMap.clear();

    return *this;
  }

  decltype(auto) getNode(UnitType Unit) {
    auto found = NodeMap.find(Unit);

    return found == NodeMap.end() ? boost::none
                                  : boost::make_optional((*found).second.get());
  }

  decltype(auto) getOrInsertNode(UnitType Unit) {
    auto &node = NodeMap[Unit];
    if (!node) {
      node = std::make_unique<NodeType>(Unit);
    }

    return node.get();
  }

  VerticesSizeType numVertices() const noexcept(noexcept(NodeMap.size())) {
    return NodeMap.size();
  }

  decltype(auto) size() const noexcept(noexcept(numVertices())) {
    return numVertices();
  }

  EdgesSizeType numEdges() const
      noexcept(noexcept(std::declval<NodeType>().numEdges())) {
    EdgesSizeType n{};
    std::for_each(std::begin(NodeMap), std::end(NodeMap),
                  [&n](const auto &e) { n += e.second.get()->numEdges(); });
    return n;
  }

  decltype(auto) begin() noexcept(noexcept(NodeMap.begin())) {
    return NodeMap.begin();
  }

  decltype(auto) begin() const noexcept(noexcept(NodeMap.begin())) {
    return NodeMap.begin();
  }

  decltype(auto) end() noexcept(noexcept(NodeMap.end())) {
    return NodeMap.end();
  }

  decltype(auto) end() const noexcept(noexcept(NodeMap.end())) {
    return NodeMap.end();
  }

  static NodeType *nodes_iterator_map(value_type &P) {
    assert(P.second.get() && "Pointer to graph node is null!");
    return P.second.get();
  }

  static const NodeType *nodes_const_iterator_map(const value_type &P) {
    assert(P.second.get() && "Pointer to graph node is null!");
    return P.second.get();
  }

  decltype(auto) nodes_begin() noexcept(
      noexcept(nodes_iterator(NodeMap.begin(), nodes_iterator_map))) {
    return nodes_iterator(NodeMap.begin(), nodes_iterator_map);
  }

  decltype(auto) nodes_begin() const
      noexcept(noexcept(const_nodes_iterator(NodeMap.begin(),
                                             nodes_const_iterator_map))) {
    return const_nodes_iterator(NodeMap.begin(), nodes_const_iterator_map);
  }

  decltype(auto) nodes_end() noexcept(
      noexcept(nodes_iterator(NodeMap.end(), nodes_iterator_map))) {
    return nodes_iterator(NodeMap.end(), nodes_iterator_map);
  }

  decltype(auto) nodes_end() const
      noexcept(noexcept(const_nodes_iterator(NodeMap.end(),
                                             nodes_const_iterator_map))) {
    return const_nodes_iterator(NodeMap.end(), nodes_const_iterator_map);
  }

  decltype(auto)
  nodes() noexcept(noexcept(llvm::make_range(nodes_begin(), nodes_end()))) {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  decltype(auto) nodes() const
      noexcept(noexcept(llvm::make_range(nodes_begin(), nodes_end()))) {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  decltype(auto) edges_begin() {
    return detail::GenericDependenceGraphEdgeIterator<
        nodes_iterator, typename NodeType::edges_iterator>{nodes_begin(),
                                                           nodes_end()};
  }

  decltype(auto) edges_end() {
    return detail::GenericDependenceGraphEdgeIterator<
        nodes_iterator, typename NodeType::edges_iterator>{nodes_end(),
                                                           nodes_end()};
  }

  decltype(auto) edges_begin() const {
    return detail::GenericDependenceGraphEdgeIterator<
        const_nodes_iterator, typename NodeType::const_edges_iterator>{
        nodes_begin(), nodes_end()};
  }

  decltype(auto) edges_end() const {
    return detail::GenericDependenceGraphEdgeIterator<
        const_nodes_iterator, typename NodeType::const_edges_iterator>{
        nodes_end(), nodes_end()};
  }

  decltype(auto)
  edges() noexcept(noexcept(llvm::make_range(edges_begin(), edges_end()))) {
    return llvm::make_range(edges_begin(), edges_end());
  }

  decltype(auto) edges() const
      noexcept(noexcept(llvm::make_range(edges_begin(), edges_end()))) {
    return llvm::make_range(edges_begin(), edges_end());
  }

  NodeType *getEntryNode() noexcept(noexcept(begin()->second.get())) {
    return begin()->second.get();
  }

  const NodeType *getEntryNode() const
      noexcept(noexcept(begin()->second.get())) {
    return begin()->second.get();
  }

  // TODO this is completely wrong
  bool compare(const GenericDependenceGraph &Other) const {
    if (numVertices() != Other.numVertices() ||
        numEdges() != Other.numEdges()) {
      return true;
    }

    for (const auto &e : *this) {
      auto found = Other.NodeMap.find(e.first);
      if (found == Other.NodeMap.end()) {
        return true;
      }

      const auto &curNode = e.second;
      const auto &foundNode = (*found).second;

      if (curNode->compare(*foundNode)) {
        return true;
      }
    }

    return false;
  }

  bool operator==(const GenericDependenceGraph &Other) const
      noexcept(noexcept(compare(Other))) {
    return !compare(Other);
  }
};

} // namespace pedigree

#endif // header
