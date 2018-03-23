//
//
//

#include "Config.hpp"

#include "Support/GenericDependenceNode.hpp"

#include "Support/GenericDependenceGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace pedigree {

#ifdef PEDIGREE_TEMPLATE_EXTERN

template class GenericDependenceNode<llvm::Instruction, BasicDependenceInfo>;
template class GenericDependenceNode<llvm::BasicBlock, BasicDependenceInfo>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::Instruction, BasicDependenceInfo>>;

template class GenericDependenceGraph<
    GenericDependenceNode<llvm::BasicBlock, BasicDependenceInfo>>;

#endif // PEDIGREE_TEMPLATE_EXTERN

} // namespace pedigree end
