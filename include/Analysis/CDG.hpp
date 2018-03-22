//
//
//

#ifndef PEDIGREE_CDG_HPP
#define PEDIGREE_CDG_HPP

#include "Config.hpp"

#include "Support/GenericDependenceGraph.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

namespace pedigree {

using ControlDependenceNode =
    GenericDependenceNode<llvm::BasicBlock, BasicDependenceInfo>;

using CDG = GenericDependenceGraph<ControlDependenceNode>;

//

using InstControlDependenceNode =
    GenericDependenceNode<llvm::Instruction, BasicDependenceInfo>;

using InstCDG = GenericDependenceGraph<InstControlDependenceNode>;

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

//

template <>
struct GraphTraits<pedigree::InstControlDependenceNode *>
    : public pedigree::LLVMDependenceNodeTraitsBase<
          pedigree::InstControlDependenceNode *> {};

template <>
struct GraphTraits<pedigree::InstCDG *>
    : public pedigree::LLVMDependenceGraphTraitsBase<pedigree::InstCDG *> {};

} // namespace llvm end

#endif // header
