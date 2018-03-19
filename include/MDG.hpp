//
//
//

#ifndef PEDIGREE_MDG_HPP
#define PEDIGREE_MDG_HPP

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

using MemoryDependenceNode = GenericDependenceNode<llvm::Instruction>;

using MDG = GenericDependenceGraph<MemoryDependenceNode>;

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::MemoryDependenceNode *>
    : public pedigree::LLVMDependenceNodeTraitsBase<
          pedigree::MemoryDependenceNode *> {};

template <>
struct GraphTraits<pedigree::MDG *>
    : public GraphTraits<pedigree::MemoryDependenceNode *> {
  using GraphTy = pedigree::MDG;

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
