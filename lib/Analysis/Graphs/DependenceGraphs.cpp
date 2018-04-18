//
//
//

#include "Config.hpp"

#include "Analysis/Graphs/DependenceGraphs.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace pedigree {

template class GenericDependenceNode<llvm::Instruction, EmptyNodeInfo,
                                     BasicDependenceInfo>;

template class GenericDependenceNode<llvm::BasicBlock, EmptyNodeInfo,
                                     BasicDependenceInfo>;

template class GenericDependenceGraph<GenericDependenceNode<
    llvm::Instruction, EmptyNodeInfo, BasicDependenceInfo>>;

template class GenericDependenceGraph<GenericDependenceNode<
    llvm::BasicBlock, EmptyNodeInfo, BasicDependenceInfo>>;

} // namespace pedigree
