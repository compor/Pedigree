//
//
//

#ifndef PEDIGREE_LLVMGRAPHTRAITSHELPERBASE_HPP
#define PEDIGREE_LLVMGRAPHTRAITSHELPERBASE_HPP

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

#include <iterator>
// using std::iterator_traits

#include <type_traits>
// using std::is_same
// using std::is_pointer
// using std::false_type

namespace pedigree {

// graph traits specializations

// generic base for easing the task of creating graph traits for graph nodes

template <typename DependenceNodeT>
struct LLVMDependenceGraphNodeTraitsHelperBase {
  static_assert(
      std::is_same<typename std::is_pointer<DependenceNodeT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename DependenceNodeT>
struct LLVMDependenceInverseGraphNodeTraitsHelperBase {
  static_assert(
      std::is_same<typename std::is_pointer<DependenceNodeT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename DependenceNodeT>
struct LLVMDependenceGraphNodeTraitsHelperBase<DependenceNodeT *> {
  using NodeType = DependenceNodeT;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(NodeRef G) { return G; }
  static unsigned size(NodeRef G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename NodeType::nodes_iterator;
  static decltype(auto) nodes_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) nodes(NodeRef G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }

  using edges_iterator = typename NodeType::edges_iterator;
  using ChildEdgeIteratorType = typename NodeType::edges_iterator;
  using EdgeRef =
      typename std::iterator_traits<ChildEdgeIteratorType>::reference;
  static decltype(auto) child_edge_begin(NodeRef G) { return G->edges_begin(); }
  static decltype(auto) child_edge_end(NodeRef G) { return G->edges_end(); }

  static decltype(auto) children_edges(NodeRef G) {
    return llvm::make_range(child_edge_begin(G), child_edge_end(G));
  }

  static NodeRef edge_dest(EdgeRef E) {
    return NodeType::nodes_iterator_map(E);
  }
};

template <typename DependenceNodeT>
struct LLVMDependenceGraphNodeTraitsHelperBase<const DependenceNodeT *> {
  using NodeType = const DependenceNodeT;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(NodeRef G) { return G; }
  static unsigned size(NodeRef G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename NodeType::const_nodes_iterator;
  static decltype(auto) nodes_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) nodes(NodeRef G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }

  using edges_iterator = typename NodeType::const_edges_iterator;
  using ChildEdgeIteratorType = typename NodeType::const_edges_iterator;
  using EdgeRef =
      typename std::iterator_traits<ChildEdgeIteratorType>::reference;
  static decltype(auto) child_edge_begin(NodeRef G) { return G->edges_begin(); }
  static decltype(auto) child_edge_end(NodeRef G) { return G->edges_end(); }

  static decltype(auto) children_edges(NodeRef G) {
    return llvm::make_range(child_edge_begin(G), child_edge_end(G));
  }

  static NodeRef edge_dest(EdgeRef E) {
    return NodeType::nodes_const_iterator_map(E);
  }
};

// inverse traits

template <typename DependenceNodeT>
struct LLVMDependenceInverseGraphNodeTraitsHelperBase<DependenceNodeT *> {
  using NodeType = DependenceNodeT;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(NodeRef G) { return G; }
  static unsigned size(NodeRef G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) child_end(NodeRef G) { return G->inverse_nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename NodeType::nodes_iterator;
  static decltype(auto) nodes_begin(NodeRef G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) nodes_end(NodeRef G) { return G->inverse_nodes_end(); }

  static decltype(auto) nodes(NodeRef G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }

  using edges_iterator = typename NodeType::inverse_edges_iterator;
  using ChildEdgeIteratorType = typename NodeType::inverse_edges_iterator;
  using EdgeRef =
      typename std::iterator_traits<ChildEdgeIteratorType>::reference;
  static decltype(auto) child_edge_begin(NodeRef G) {
    return G->inverse_edges_begin();
  }
  static decltype(auto) child_edge_end(NodeRef G) {
    return G->inverse_edges_end();
  }

  static decltype(auto) children_edges(NodeRef G) {
    return llvm::make_range(child_edge_begin(G), child_edge_end(G));
  }

  static NodeRef edge_dest(EdgeRef E) { return E; }
};

template <typename DependenceNodeT>
struct LLVMDependenceInverseGraphNodeTraitsHelperBase<const DependenceNodeT *> {
  using NodeType = const DependenceNodeT;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(NodeRef G) { return G; }
  static unsigned size(NodeRef G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) child_end(NodeRef G) { return G->inverse_nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename NodeType::const_nodes_iterator;
  static decltype(auto) nodes_begin(NodeRef G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) nodes_end(NodeRef G) { return G->inverse_nodes_end(); }

  static decltype(auto) nodes(NodeRef G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }

  using edges_iterator = typename NodeType::const_inverse_edges_iterator;
  using ChildEdgeIteratorType = typename NodeType::const_inverse_edges_iterator;
  using EdgeRef =
      typename std::iterator_traits<ChildEdgeIteratorType>::reference;
  static decltype(auto) child_edge_begin(NodeRef G) {
    return G->inverse_edges_begin();
  }
  static decltype(auto) child_edge_end(NodeRef G) {
    return G->inverse_edges_end();
  }

  static decltype(auto) children_edges(NodeRef G) {
    return llvm::make_range(child_edge_begin(G), child_edge_end(G));
  }

  static NodeRef edge_dest(EdgeRef E) { return E; }
};

// generic base for easing the task of creating graph traits for graphs

template <typename GraphT> struct LLVMDependenceGraphTraitsHelperBase {
  static_assert(
      std::is_same<typename std::is_pointer<GraphT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename GraphT> struct LLVMDependenceInverseGraphTraitsHelperBase {
  static_assert(
      std::is_same<typename std::is_pointer<GraphT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename GraphT>
struct LLVMDependenceGraphTraitsHelperBase<GraphT *> {
  using NodeType = typename GraphT::NodeType;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(GraphT *G) { return G->getEntryNode(); }
  static unsigned size(GraphT *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename GraphT::nodes_iterator;
  static decltype(auto) nodes_begin(GraphT *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(GraphT *G) { return G->nodes_end(); }

  static decltype(auto) nodes(GraphT *G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }
};

template <typename GraphT>
struct LLVMDependenceGraphTraitsHelperBase<const GraphT *> {
  using NodeType = const typename GraphT::NodeType;
  using NodeRef = NodeType *;

  static const NodeRef getEntryNode(const GraphT *G) {
    return G->getEntryNode();
  }
  static unsigned size(const GraphT *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename GraphT::const_nodes_iterator;
  static decltype(auto) nodes_begin(const GraphT *G) {
    return G->nodes_begin();
  }
  static decltype(auto) nodes_end(const GraphT *G) { return G->nodes_end(); }

  static decltype(auto) nodes(const GraphT *G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }
};

// inverse traits

template <typename GraphT>
struct LLVMDependenceInverseGraphTraitsHelperBase<GraphT *> {
  using NodeType = typename GraphT::NodeType;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(GraphT *G) { return G->getEntryNode(); }
  static unsigned size(GraphT *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) child_end(NodeRef G) { return G->inverse_nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename GraphT::nodes_iterator;
  static decltype(auto) nodes_begin(GraphT *G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) nodes_end(GraphT *G) { return G->inverse_nodes_end(); }

  static decltype(auto) nodes(GraphT *G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }
};

template <typename GraphT>
struct LLVMDependenceInverseGraphTraitsHelperBase<const GraphT *> {
  using NodeType = const typename GraphT::NodeType;
  using NodeRef = NodeType *;

  static const NodeRef getEntryNode(const GraphT *G) {
    return G->getEntryNode();
  }
  static unsigned size(const GraphT *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) child_end(NodeRef G) { return G->inverse_nodes_end(); }

  static decltype(auto) children(NodeRef G) {
    return llvm::make_range(child_begin(G), child_end(G));
  }

  using nodes_iterator = typename GraphT::const_nodes_iterator;
  static decltype(auto) nodes_begin(const GraphT *G) {
    return G->inverse_nodes_begin();
  }
  static decltype(auto) nodes_end(const GraphT *G) {
    return G->inverse_nodes_end();
  }

  static decltype(auto) nodes(const GraphT *G) {
    return llvm::make_range(nodes_begin(G), nodes_end(G));
  }
};

} // namespace pedigree

#endif // header
