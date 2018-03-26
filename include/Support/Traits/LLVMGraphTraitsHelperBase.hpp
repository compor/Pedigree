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

  static NodeType *getEntryNode(NodeType *G) { return G; }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  static decltype(auto) children(NodeType *G) { return G->nodes(); }

  using nodes_iterator = typename NodeType::nodes_iterator;
  static decltype(auto) nodes_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeType *G) { return G->nodes_end(); }

  static decltype(auto) nodes(NodeType *G) { return G->nodes(); }

  using edges_iterator = typename NodeType::edges_iterator;
  static decltype(auto) edges_begin(NodeType *G) { return G->edges_begin(); }
  static decltype(auto) edges_end(NodeType *G) { return G->edges_end(); }

  static decltype(auto) edges(NodeType *G) { return G->edges(); }
};

template <typename DependenceNodeT>
struct LLVMDependenceNodeTraitsHelperBase<const DependenceNodeT *> {
  using NodeType = const DependenceNodeT;

  static NodeType *getEntryNode(NodeType *G) { return G; }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  static decltype(auto) children(NodeType *G) { return G->nodes(); }

  using nodes_iterator = typename NodeType::const_nodes_iterator;
  static decltype(auto) nodes_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(NodeType *G) { return G->nodes_end(); }

  static decltype(auto) nodes(NodeType *G) { return G->nodes(); }

  using edges_iterator = typename NodeType::const_edges_iterator;
  static decltype(auto) edges_begin(NodeType *G) { return G->edges_begin(); }
  static decltype(auto) edges_end(NodeType *G) { return G->edges_end(); }

  static decltype(auto) edges(NodeType *G) { return G->edges(); }
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

  static NodeType *getEntryNode(NodeType *G) { return G->getEntryNode(); }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  static decltype(auto) children(NodeType *G) { return G->nodes(); }

  using nodes_iterator = typename GraphT::nodes_iterator;
  static decltype(auto) nodes_begin(GraphT *G) { return G->nodes_begin(); }
  static decltype(auto) nodes_end(GraphT *G) { return G->nodes_end(); }

  static decltype(auto) nodes(GraphT *G) { return G->nodes(); }
};

template <typename GraphT>
struct LLVMDependenceGraphTraitsHelperBase<const GraphT *> {
  using NodeType = const typename GraphT::NodeType;

  static const NodeType *getEntryNode(NodeType *G) { return G->getEntryNode(); }
  static unsigned size(NodeType *G) { return G->size(); }

  using ChildIteratorType = typename NodeType::const_nodes_iterator;
  static decltype(auto) child_begin(NodeType *G) { return G->nodes_begin(); }
  static decltype(auto) child_end(NodeType *G) { return G->nodes_end(); }

  static decltype(auto) children(NodeType *G) { return G->nodes(); }

  using nodes_iterator = typename GraphT::const_nodes_iterator;
  static decltype(auto) nodes_begin(const GraphT *G) {
    return G->nodes_begin();
  }
  static decltype(auto) nodes_end(const GraphT *G) { return G->nodes_end(); }

  static decltype(auto) nodes(const GraphT *G) { return G->nodes(); }
};

} // namespace pedigree end

#endif // header
