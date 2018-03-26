//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Config.hpp"

#include "Dependence.hpp"

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

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

namespace pedigree {

template <typename NodeT>
class GenericDependenceGraph
    : boost::equality_comparable<GenericDependenceGraph<NodeT>> {
public:
  using NodeType = NodeT;

private:
  using UnderlyingType = typename NodeType::UnderlyingType;
  using NodeMapType = std::map<UnderlyingType, std::unique_ptr<NodeType>>;

  NodeMapType NodeMap;

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

  GenericDependenceGraph() = default;
  GenericDependenceGraph(const GenericDependenceGraph &) = delete;
  GenericDependenceGraph &operator=(const GenericDependenceGraph &) = delete;

  explicit GenericDependenceGraph(GenericDependenceGraph &&Other)
      : NodeMap(std::move(Other.NodeMap)) {
    Other.NodeMap.clear();
  }

  GenericDependenceGraph &operator=(GenericDependenceGraph &&Other) {
    NodeMap = std::move(Other.NodeMap);
    Other.NodeMap.clear();

    return *this;
  }

  decltype(auto) getOrInsertNode(UnderlyingType Under) {
    auto &node = NodeMap[Under];
    if (!node)
      node = std::make_unique<NodeType>(Under);

    return node.get();
  }

  VerticesSizeType numVertices() const { return NodeMap.size(); }
  decltype(auto) size() const { return numVertices(); }

  EdgesSizeType numEdges() const {
    EdgesSizeType n{};
    std::for_each(std::begin(NodeMap), std::end(NodeMap),
                  [&n](const auto &e) { n += e.second.get()->numEdges(); });
    return n;
  }

  inline decltype(auto) begin() { return NodeMap.begin(); }
  inline decltype(auto) begin() const { return NodeMap.begin(); }
  inline decltype(auto) end() { return NodeMap.end(); }
  inline decltype(auto) end() const { return NodeMap.end(); }

  static NodeType *nodes_iterator_map(value_type &P) {
    assert(P.second.get() && "Pointer to graph node is null!");
    return P.second.get();
  }

  static const NodeType *nodes_const_iterator_map(const value_type &P) {
    assert(P.second.get() && "Pointer to graph node is null!");
    return P.second.get();
  }

  using nodes_iterator =
      llvm::mapped_iterator<iterator, std::function<NodeType *(value_type &)>>;

  using const_nodes_iterator = llvm::mapped_iterator<
      const_iterator, std::function<const NodeType *(const value_type &)>>;

  inline decltype(auto) nodes_begin() {
    return nodes_iterator(NodeMap.begin(), nodes_iterator_map);
  }

  inline decltype(auto) nodes_begin() const {
    return const_nodes_iterator(NodeMap.begin(), nodes_const_iterator_map);
  }

  inline decltype(auto) nodes_end() {
    return nodes_iterator(NodeMap.end(), nodes_iterator_map);
  }

  inline decltype(auto) nodes_end() const {
    return const_nodes_iterator(NodeMap.end(), nodes_const_iterator_map);
  }

  inline decltype(auto) nodes() {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  inline decltype(auto) nodes() const {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  NodeType *getEntryNode() { return begin()->second.get(); }
  const NodeType *getEntryNode() const { return begin()->second.get(); }

  bool compare(const GenericDependenceGraph &Other) const {
    if (numVertices() != Other.numVertices() || numEdges() != Other.numEdges())
      return true;

    for (const auto &e : *this) {
      auto found = Other.NodeMap.find(e.first);
      if (found == Other.NodeMap.end())
        return true;

      const auto &curNode = e.second;
      const auto &foundNode = (*found).second;

      if (curNode->compare(*foundNode))
        return true;
    }

    return false;
  }

  bool operator==(const GenericDependenceGraph &Other) const {
    return !compare(Other);
  }
};

} // namespace pedigree end

#endif // header
