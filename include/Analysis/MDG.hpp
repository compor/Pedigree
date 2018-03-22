//
//
//

#ifndef PEDIGREE_MDG_HPP
#define PEDIGREE_MDG_HPP

#include "Config.hpp"

#include "Support/GenericDependenceGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

namespace pedigree {

using MemoryDependenceNode =
    GenericDependenceNode<llvm::Instruction, BasicDependenceInfo>;

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
    : public pedigree::LLVMDependenceGraphTraitsBase<pedigree::MDG *> {};

} // namespace llvm end

#endif // header
