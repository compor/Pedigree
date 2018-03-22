//
//
//

#ifndef PEDIGREE_CDG_HPP
#define PEDIGREE_CDG_HPP

#include "Config.hpp"

#include "Support/GenericDependenceGraph.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace pedigree {

using ControlDependenceNode =
    GenericDependenceNode<llvm::BasicBlock, BasicDependenceInfo>;

using CDG = GenericDependenceGraph<ControlDependenceNode>;

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::ControlDependenceNode *>
    : public pedigree::LLVMDependenceNodeTraitsBase<
          pedigree::ControlDependenceNode *> {};

template <>
struct GraphTraits<pedigree::CDG *>
    : public pedigree::LLVMDependenceGraphTraitsBase<pedigree::CDG *> {};

} // namespace llvm end

#endif // header
