//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Config.hpp"

#include "GenericDependenceNode.hpp"

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include "boost/operators.hpp"
// using boost::equality_comparable

#include <map>
// using std::map

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <algorithm>
// using std::for_each

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
  inline decltype(auto) end() { return NodeMap.end(); }
  inline decltype(auto) begin() const { return NodeMap.begin(); }
  inline decltype(auto) end() const { return NodeMap.end(); }

  static NodeType *nodes_iterator_map(value_type &P) {
    assert(P.second.get() && "Pointer to graph node is null!");
    return P.second.get();
  }

  using nodes_iterator =
      llvm::mapped_iterator<iterator, std::function<NodeType *(value_type &)>>;

  inline decltype(auto) nodes_begin() {
    return nodes_iterator(NodeMap.begin(), nodes_iterator_map);
  }

  inline decltype(auto) nodes_end() {
    return nodes_iterator(NodeMap.end(), nodes_iterator_map);
  }

  const NodeType *getEntryNode() const { return begin()->second.get(); }
  NodeType *getEntryNode() { return begin()->second.get(); }

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

// graph traits specializations

// generic base for easing the task of creating graph traits for graph nodes

template <typename GraphT> struct LLVMDependenceGraphTraitsBase {
  static_assert(
      std::is_same<typename std::is_pointer<GraphT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename GraphT> struct LLVMDependenceGraphTraitsBase<GraphT *> {
  using NodeType = typename GraphT::NodeType;

  static NodeType *getEntryNode(NodeType *G) { return G->getEntryNode(); }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  using nodes_iterator = typename GraphT::nodes_iterator;
  static decltype(auto) nodes_begin(GraphT *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(GraphT *G) { return G->nodes_end(); }
};

} // namespace pedigree end

#endif // header
