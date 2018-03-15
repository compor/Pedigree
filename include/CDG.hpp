//
//
//

#ifndef PEDIGREE_CDG_HPP
#define PEDIGREE_CDG_HPP

#include "Dependence.hpp"

#include "GenericDependenceGraph.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

using ControlDependenceNode = GenericDependenceNode<llvm::BasicBlock>;

using CDG = GenericDependenceGraph<ControlDependenceNode>;

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::ControlDependenceNode *>
    : public pedigree::DependenceNodeGraphTraitsBase<
          pedigree::ControlDependenceNode *> {};

template <>
struct GraphTraits<pedigree::CDG *>
    : public GraphTraits<pedigree::ControlDependenceNode *> {
  using GraphTy = pedigree::CDG;

  using NodePairTy = GraphTy::value_type;

  // TODO we might need to hide the implementation detail of using a smart
  // pointer in the graph class by mapping the iterator provided by it in the
  // graph class itself
  // TODO this also forces us to use a ref to pass the node pair since the
  // current smart pointer used disallows copy
  static NodeType &NodeDeref(NodePairTy &P) { return *P.second.get(); }
  using NodeDerefFuncTy = std::function<decltype(NodeDeref)>;

  using nodes_iterator =
      llvm::mapped_iterator<GraphTy::iterator, NodeDerefFuncTy>;

  static NodeType *getEntryNode(GraphTy *G) { return G->getEntryNode(); }

  static nodes_iterator nodes_begin(GraphTy *G) {
    using std::begin;
    return llvm::map_iterator(begin(*G), NodeDerefFuncTy(NodeDeref));
  }

  static nodes_iterator nodes_end(GraphTy *G) {
    using std::end;
    return llvm::map_iterator(end(*G), NodeDerefFuncTy(NodeDeref));
  }

  static unsigned size(GraphTy *G) {
    return static_cast<unsigned>(G->numVertices());
  }
};

} // namespace llvm end

#endif // header
