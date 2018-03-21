//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCEGRAPH_HPP
#define PEDIGREE_GENERICDEPENDENCEGRAPH_HPP

#include "Dependence.hpp"

#include "llvm/ADT/SmallPtrSet.h"
// using llvm::SmallPtrSet

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include "boost/operators.hpp"
// using boost::equality_comparable

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

template <typename T, typename EdgeInfoT = NoDependenceInfo>
class GenericDependenceNode
    : boost::equality_comparable<GenericDependenceNode<T, EdgeInfoT>> {
public:
  using NodeType = GenericDependenceNode;
  using UnderlyingType = T *;
  using ConstUnderlyingType = const T *;
  using EdgeInfoType = EdgeInfoT;

private:
  using DependenceRecordType = std::pair<GenericDependenceNode *, EdgeInfoType>;
  using EdgeStorageType = std::vector<DependenceRecordType>;

  inline void incrementDependeeCount() { ++DependeeCount; }
  inline void decrementDependeeCount() { --DependeeCount; }

  EdgeStorageType Edges;
  UnderlyingType Underlying;
  unsigned DependeeCount;

public:
  using value_type = typename EdgeStorageType::value_type;
  using EdgesSizeType = typename EdgeStorageType::size_type;

  using iterator = typename EdgeStorageType::iterator;
  using const_iterator = typename EdgeStorageType::const_iterator;

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

  void addDependentNode(GenericDependenceNode *Node, const EdgeInfoType &Info) {
    Edges.emplace_back(Node, Info);
    Node->incrementDependeeCount();
  }

  const EdgeInfoType &getEdgeInfo(NodeType *Node) const {
    auto found =
        std::find_if(Edges.begin(), Edges.end(),
                     [&Node](const auto &e) { return Node == e.first; });

    assert(found != Edges.end() && "Edge could not be found!");

    return (*found).second;
  }

  EdgesSizeType numEdges() const { return Edges.size(); }
  decltype(auto) size() const { return numEdges(); }

  inline decltype(auto) begin() { return Edges.begin(); }
  inline decltype(auto) end() { return Edges.end(); }
  inline decltype(auto) begin() const { return Edges.begin(); }
  inline decltype(auto) end() const { return Edges.end(); }

  static GenericDependenceNode *nodes_iterator_map(value_type &P) {
    assert(P.first && "Pointer to graph node is null!");
    return P.first;
  }

  using nodes_iterator = llvm::mapped_iterator<
      iterator, std::function<GenericDependenceNode *(value_type &)>>;

  inline decltype(auto) nodes_begin() {
    return nodes_iterator(Edges.begin(), nodes_iterator_map);
  }

  inline decltype(auto) nodes_end() {
    return nodes_iterator(Edges.end(), nodes_iterator_map);
  }

  inline unsigned getDependeeCount() const { return DependeeCount; }

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

  explicit GenericDependenceGraph(const GenericDependenceGraph &&Other)
      : NodeMap(std::move(Other.NodeMap)) {
    Other.NodeMap.clear();
  }

  GenericDependenceGraph &operator=(const GenericDependenceGraph &&Other) {
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

template <typename DependenceNodeT> struct LLVMDependenceNodeTraitsBase {
  static_assert(
      std::is_same<typename std::is_pointer<DependenceNodeT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename DependenceNodeT>
struct LLVMDependenceNodeTraitsBase<DependenceNodeT *> {
  using NodeType = DependenceNodeT;

  static NodeType *getEntryNode(NodeType *G) { return G; }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  using nodes_iterator = typename NodeType::nodes_iterator;
  static decltype(auto) nodes_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeType *G) { return G->nodes_end(); }
};

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
