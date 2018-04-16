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

template class GenericDependenceNode<llvm::Instruction, void,
                                     BasicDependenceInfo>;
template class GenericDependenceNode<llvm::BasicBlock, void,
                                     BasicDependenceInfo>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::Instruction, void, BasicDependenceInfo>>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::BasicBlock, void, BasicDependenceInfo>>;

} // namespace pedigree
