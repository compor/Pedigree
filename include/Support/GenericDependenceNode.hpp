//
//
//

#ifndef PEDIGREE_GENERICDEPENDENCENODE_HPP
#define PEDIGREE_GENERICDEPENDENCENODE_HPP

#include "Config.hpp"

#include "Support/GenericDependenceGraph.hpp"

#include "Support/Traits/TypeTraits.hpp"

#include "Analysis/Info/EmptyInfo.hpp"

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
// using std::forward

#include <functional>
// using std::function

#include <type_traits>
// using std::enable_if
// using std::is_void

namespace pedigree {

namespace detail {

template <typename NodeT, typename InfoT>
struct EdgeRecordImpl : private InfoT::value_type {
  using node_type = NodeT;
  using info_type = InfoT;
  node_type node;

  template <typename... Args>
  EdgeRecordImpl(node_type node, Args &&... args)
      : info_type::value_type(std::forward<Args>(args)...),
        node(std::move(node)) {}

  typename info_type::value_type &&info() && noexcept {
    return std::move(*this);
  }

  const typename info_type::value_type &info() const &noexcept { return *this; }

  typename info_type::value_type &info() & noexcept { return *this; }
};

} // namespace detail

template <typename T, typename NodeInfoT = EmptyInfo,
          typename EdgeInfoT = EmptyInfo>
class GenericDependenceNode
    : NodeInfoT::value_type,
      boost::equality_comparable<
          GenericDependenceNode<T, NodeInfoT, EdgeInfoT>> {

  using ContainingGraphType =
      GenericDependenceGraph<GenericDependenceNode<T, NodeInfoT, EdgeInfoT>>;

  friend ContainingGraphType;

public:
  using NodeType = GenericDependenceNode;
  using UnitType = T *;
  using ConstUnitType = const T *;
  using NodeInfoType = NodeInfoT;
  using EdgeInfoType = EdgeInfoT;

private:
  using DependenceRecordType = detail::EdgeRecordImpl<NodeType *, EdgeInfoType>;
  using OutEdgeStorageType = std::vector<DependenceRecordType>;
  using InEdgeStorageType = llvm::SmallPtrSet<NodeType *, 8>;

  UnitType Unit;
  mutable InEdgeStorageType InEdges;
  OutEdgeStorageType OutEdges;

public:
  using value_type = typename OutEdgeStorageType::value_type;
  using InEdgesSizeType = typename InEdgeStorageType::size_type;
  using OutEdgesSizeType = typename OutEdgeStorageType::size_type;

  using iterator = typename OutEdgeStorageType::iterator;
  using const_iterator = typename OutEdgeStorageType::const_iterator;

  using nodes_iterator =
      llvm::mapped_iterator<iterator, std::function<NodeType *(value_type &)>>;

  using const_nodes_iterator = llvm::mapped_iterator<
      const_iterator, std::function<const NodeType *(const value_type &)>>;

  using inverse_nodes_iterator = typename OutEdgeStorageType::iterator;
  using const_inverse_nodes_iterator =
      typename OutEdgeStorageType::const_iterator;

  using edges_iterator = iterator;
  using const_edges_iterator = const_iterator;

  template <typename... Args>
  explicit GenericDependenceNode(UnitType Unit, Args &&... args) noexcept
      : NodeInfoType::value_type(std::forward<Args>(args)...), Unit(Unit) {}

  GenericDependenceNode(const GenericDependenceNode &) = delete;
  GenericDependenceNode &operator=(const GenericDependenceNode &) = delete;

  GenericDependenceNode(GenericDependenceNode &&Other) noexcept(
      are_all_nothrow_move_constructible_v<decltype(Unit), decltype(InEdges),
                                           decltype(OutEdges),
                                           typename NodeInfoType::value_type>)
      : NodeInfoType::value_type(std::move(Other)), Unit(std::move(Other.Unit)),
        InEdges(std::move(Other.InEdges)), OutEdges(std::move(Other.OutEdges)) {
    Other.Unit = {};
    Other.InEdges.clear();
    Other.OutEdges.clear();
  }

  GenericDependenceNode &operator=(GenericDependenceNode &&Other) noexcept(
      are_all_nothrow_move_assignable_v<decltype(Unit), decltype(InEdges),
                                        decltype(OutEdges),
                                        typename NodeInfoType::value_type>) {
    NodeInfoType::value_type::operator=(std::move(Other));

    Unit = std::move(Other.Unit);
    InEdges = std::move(Other.InEdges);
    OutEdges = std::move(Other.OutEdges);

    Other.Unit = {};
    Other.InEdges.clear();
    Other.OutEdges.clear();

    return *this;
  };

  typename NodeInfoType::value_type &&info() && noexcept {
    return std::move(*this);
  }

  const typename NodeInfoType::value_type &info() const &noexcept {
    return *this;
  }

  typename NodeInfoType::value_type &info() & noexcept { return *this; }

  UnitType unit() const noexcept { return Unit; }

  bool hasEdgeWith(const NodeType *Node) const {
    return OutEdges.end() != getEdgeWith(Node);
  }

  void addDependentNode(const NodeType *Node,
                        typename EdgeInfoType::value_type Info = {}) {
    OutEdges.emplace_back(const_cast<NodeType *>(Node), std::move(Info));
    Node->InEdges.insert(this);
  }

  boost::optional<const typename EdgeInfoType::value_type &>
  getEdgeInfo(const NodeType *Node) const {
    auto found = getEdgeWith(Node);

    return found != OutEdges.end()
               ? boost::optional<const typename EdgeInfoType::value_type &>(
                     (*found).info())
               : boost::none;
  }

  bool setEdgeInfo(const NodeType *Node,
                   typename EdgeInfoType::value_type Info = {}) {
    auto found = getEdgeWith(Node);

    if (found == OutEdges.end()) {
      return false;
    }

    auto &info = (*found).info();
    info = std::move(Info);

    return true;
  }

  OutEdgesSizeType numOutEdges() const noexcept(noexcept(OutEdges.size())) {
    return OutEdges.size();
  }

  InEdgesSizeType numInEdges() const noexcept(noexcept(InEdges.size())) {
    return InEdges.size();
  }

  decltype(auto) size() const noexcept(noexcept(numOutEdges())) {
    return numOutEdges();
  }

  decltype(auto) begin() noexcept(noexcept(OutEdges.begin())) {
    return OutEdges.begin();
  }

  decltype(auto) begin() const noexcept(noexcept(OutEdges.begin())) {
    return OutEdges.begin();
  }

  decltype(auto) end() noexcept(noexcept(OutEdges.end())) {
    return OutEdges.end();
  }

  decltype(auto) end() const noexcept(noexcept(OutEdges.end())) {
    return OutEdges.end();
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
  nodes_begin() noexcept(noexcept(nodes_iterator(OutEdges.begin(), {}))) {
    return nodes_iterator(OutEdges.begin(), nodes_iterator_map);
  }

  decltype(auto) nodes_begin() const
      noexcept(noexcept(const_nodes_iterator(OutEdges.begin(), {}))) {
    return const_nodes_iterator(OutEdges.begin(), nodes_const_iterator_map);
  }

  decltype(auto)
  nodes_end() noexcept(noexcept(nodes_iterator(OutEdges.end(), {}))) {
    return nodes_iterator(OutEdges.end(), nodes_iterator_map);
  }

  decltype(auto) nodes_end() const
      noexcept(noexcept(const_nodes_iterator(OutEdges.end(), {}))) {
    return const_nodes_iterator(OutEdges.end(), nodes_const_iterator_map);
  }

  decltype(auto)
  nodes() noexcept(noexcept(llvm::make_range(nodes_begin(), nodes_end()))) {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  decltype(auto) nodes() const
      noexcept(noexcept(llvm::make_range(nodes_begin(), nodes_end()))) {
    return llvm::make_range(nodes_begin(), nodes_end());
  }

  decltype(auto) inverse_nodes_begin() noexcept(noexcept(InEdges.begin())) {
    return InEdges.begin();
  }

  decltype(auto) inverse_nodes_begin() const
      noexcept(noexcept(InEdges.begin())) {
    return InEdges.begin();
  }

  decltype(auto) inverse_nodes_end() noexcept(noexcept(InEdges.end())) {
    return InEdges.end();
  }

  decltype(auto) inverse_nodes_end() const noexcept(noexcept(InEdges.end())) {
    return InEdges.end();
  }

  decltype(auto) inverse_nodes() noexcept(
      noexcept(llvm::make_range(inverse_nodes_begin(), inverse_nodes_end()))) {
    return llvm::make_range(inverse_nodes_begin(), inverse_nodes_end());
  }

  decltype(auto) inverse_nodes() const
      noexcept(noexcept(llvm::make_range(inverse_nodes_begin(),
                                         inverse_nodes_end()))) {
    return llvm::make_range(inverse_nodes_begin(), inverse_nodes_end());
  }

  bool compare(const GenericDependenceNode &Other) const {
    if (this->numOutEdges() != Other.numOutEdges()) {
      return true;
    }

    llvm::SmallPtrSet<ConstUnitType, 8> otherChildren;
    for (const auto &e : Other)
      otherChildren.insert(e.node->unit());

    for (const auto &e : *this)
      if (otherChildren.count(e.node->unit()) == 0) {
        return true;
      }

    return false;
  }

  bool operator==(const GenericDependenceNode &Other) const {
    return !compare(Other);
  }

private:
  const_iterator getEdgeWith(const NodeType *Node) const {
    return std::find_if(OutEdges.begin(), OutEdges.end(),
                        [&Node](const auto &e) { return Node == e.node; });
  }

  iterator getEdgeWith(const NodeType *Node) {
    return std::find_if(OutEdges.begin(), OutEdges.end(),
                        [&Node](const auto &e) { return Node == e.node; });
  }
};

} // namespace pedigree

#endif // header
