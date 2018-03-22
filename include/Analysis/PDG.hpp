//
//
//

#ifndef PEDIGREE_PDG_HPP
#define PEDIGREE_PDG_HPP

#include "Config.hpp"

#include "Support/GenericDependenceGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

namespace pedigree {

using ProgramDependenceNode =
    GenericDependenceNode<llvm::Instruction, BasicDependenceInfo>;

using PDG = GenericDependenceGraph<ProgramDependenceNode>;

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::ProgramDependenceNode *>
    : public pedigree::LLVMDependenceNodeTraitsBase<
          pedigree::ProgramDependenceNode *> {};

template <>
struct GraphTraits<pedigree::PDG *>
    : public pedigree::LLVMDependenceGraphTraitsBase<pedigree::PDG *> {};

} // namespace llvm end

#endif // header
