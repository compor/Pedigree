//
//
//

#ifndef PEDIGREE_DDG_HPP
#define PEDIGREE_DDG_HPP

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

using DataDependenceNode =
    GenericDependenceNode<llvm::Instruction, BasicDependenceInfo>;

using DDG = GenericDependenceGraph<DataDependenceNode>;

} // namespace pedigree end

namespace llvm {

// graph traits specializations

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

template <>
struct GraphTraits<pedigree::DataDependenceNode *>
    : public pedigree::LLVMDependenceNodeTraitsBase<
          pedigree::DataDependenceNode *> {};

template <>
struct GraphTraits<pedigree::DDG *>
    : public pedigree::LLVMDependenceGraphTraitsBase<pedigree::DDG *> {};

} // namespace llvm end

#endif // header
