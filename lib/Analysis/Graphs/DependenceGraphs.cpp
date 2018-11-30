//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace pedigree {

template class GenericDependenceNode<llvm::Instruction, GeneralInfo,
                                     BasicDependenceInfo>;

template class GenericDependenceNode<llvm::BasicBlock, GeneralInfo,
                                     BasicDependenceInfo>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::Instruction, GeneralInfo, BasicDependenceInfo>>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::BasicBlock, GeneralInfo, BasicDependenceInfo>>;

} // namespace pedigree
