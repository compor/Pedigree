//
//
//

#include "pedigree/Config.hpp"

#include "pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace pedigree {

template class GenericDependenceNode<llvm::Instruction, UIDInfo,
                                     BasicDependenceInfo>;

template class GenericDependenceNode<llvm::BasicBlock, UIDInfo,
                                     BasicDependenceInfo>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::Instruction, UIDInfo, BasicDependenceInfo>>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::BasicBlock, UIDInfo, BasicDependenceInfo>>;

} // namespace pedigree
