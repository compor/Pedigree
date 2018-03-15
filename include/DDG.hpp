//
//
//

#ifndef PEDIGREE_DDG_HPP
#define PEDIGREE_DDG_HPP

#include "Dependence.hpp"

#include "GenericDependenceGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/ADT/STLExtras.h"
// using llvm::mapped_iterator

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

using DataDependenceNode = GenericDependenceNode<llvm::Instruction>;

using DDG = GenericDependenceGraph<DataDependenceNode>;

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::DataDependenceNode *>
    : public pedigree::DependenceNodeGraphTraitsBase<
          pedigree::DataDependenceNode *> {};

template <>
struct GraphTraits<pedigree::DDG *>
    : public GraphTraits<pedigree::DataDependenceNode *> {
  using GraphTy = pedigree::DDG;

  using NodePairTy = GraphTy::value_type;

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
