//
//
//

#ifndef PEDIGREE_LLVMGRAPHTRAITSHELPERBASE_HPP
#define PEDIGREE_LLVMGRAPHTRAITSHELPERBASE_HPP

#include <type_traits>
// using std::is_same
// using std::is_pointer
// using std::false_type

namespace pedigree {

// graph traits specializations

// generic base for easing the task of creating graph traits for graph nodes

template <typename DependenceNodeT> struct LLVMDependenceNodeTraitsHelperBase {
  static_assert(
      std::is_same<typename std::is_pointer<DependenceNodeT>::type,
                   std::false_type::type>::value,
      "Traits class needs to be partially specialized for pointer types!");
};

template <typename DependenceNodeT>
struct LLVMDependenceNodeTraitsHelperBase<DependenceNodeT *> {
  using NodeType = DependenceNodeT;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(NodeRef G) { return G; }
  static unsigned size(NodeRef G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) children(NodeRef G) { return G->nodes(); }

  using nodes_iterator = typename NodeType::nodes_iterator;
  static decltype(auto) nodes_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) nodes(NodeRef G) { return G->nodes(); }

  using edges_iterator = typename NodeType::edges_iterator;
  static decltype(auto) edges_begin(NodeRef G) { return G->edges_begin(); }
  static decltype(auto) edges_end(NodeRef G) { return G->edges_end(); }

  static decltype(auto) edges(NodeRef G) { return G->edges(); }
};

template <typename DependenceNodeT>
struct LLVMDependenceNodeTraitsHelperBase<const DependenceNodeT *> {
  using NodeType = const DependenceNodeT;
  using NodeRef = NodeType *;

  static NodeRef getEntryNode(NodeRef G) { return G; }
  static unsigned size(NodeRef G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) children(NodeRef G) { return G->nodes(); }

  using nodes_iterator = typename NodeType::const_nodes_iterator;
  static decltype(auto) nodes_begin(NodeRef G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeRef G) { return G->nodes_end(); }

  static decltype(auto) nodes(NodeRef G) { return G->nodes(); }

  using edges_iterator = typename NodeType::const_edges_iterator;
  static decltype(auto) edges_begin(NodeRef G) { return G->edges_begin(); }
  static decltype(auto) edges_end(NodeRef G) { return G->edges_end(); }

  static decltype(auto) edges(NodeRef G) { return G->edges(); }
};

// generic base for easing the task of creating graph traits for graphs

template <typename GraphT> struct LLVMDependenceGraphTraitsHelperBase {
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

  static decltype(auto) children(NodeRef G) { return G->nodes(); }

  using nodes_iterator = typename GraphT::nodes_iterator;
  static decltype(auto) nodes_begin(GraphT *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(GraphT *G) { return G->nodes_end(); }

  static decltype(auto) nodes(GraphT *G) { return G->nodes(); }
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

  static decltype(auto) children(NodeRef G) { return G->nodes(); }

  using nodes_iterator = typename GraphT::const_nodes_iterator;
  static decltype(auto) nodes_begin(const GraphT *G) {
    return G->nodes_begin();
  }
  static decltype(auto) nodes_end(const GraphT *G) { return G->nodes_end(); }

  static decltype(auto) nodes(const GraphT *G) { return G->nodes(); }
};

} // namespace pedigree

#endif // header
