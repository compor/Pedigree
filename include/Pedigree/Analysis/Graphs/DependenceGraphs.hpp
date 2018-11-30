//
//
//

#ifndef PEDIGREE_DEPENDENCEGRAPHS_HPP
#define PEDIGREE_DEPENDENCEGRAPHS_HPP

#include "Pedigree/Config.hpp"

#include "Pedigree/Support/GenericDependenceNode.hpp"

#include "Pedigree/Support/GenericDependenceGraph.hpp"

#include "Pedigree/Support/Traits/LLVMGraphTraitsHelperBase.hpp"

#include "Pedigree/Support/Traits/LLVMDOTGraphTraitsHelperBase.hpp"

#include "Pedigree/Analysis/Info/NodeInfo/GeneralInfo.hpp"

#include "Pedigree/Analysis/Info/EdgeInfo/BasicDependenceInfo.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/ADT/GraphTraits.h"
// using llvm::GraphTraits

namespace pedigree {

extern template class GenericDependenceNode<llvm::Instruction, GeneralInfo,
                                            BasicDependenceInfo>;

extern template class GenericDependenceNode<llvm::BasicBlock, GeneralInfo,
                                            BasicDependenceInfo>;

extern template class GenericDependenceGraph<
    GenericDependenceNode<llvm::Instruction, GeneralInfo, BasicDependenceInfo>>;

extern template class GenericDependenceGraph<
    GenericDependenceNode<llvm::BasicBlock, GeneralInfo, BasicDependenceInfo>>;

//
// aliases for llvm::Instruction

template <typename EdgeInfoT>
using InstructionGenericDependenceNode =
    GenericDependenceNode<llvm::Instruction, GeneralInfo, EdgeInfoT>;

using InstructionDependenceNode =
    InstructionGenericDependenceNode<BasicDependenceInfo>;

template <typename EdgeInfoT>
using InstructionGenericDependenceGraph =
    GenericDependenceGraph<InstructionGenericDependenceNode<EdgeInfoT>>;

using InstructionDependenceGraph =
    InstructionGenericDependenceGraph<BasicDependenceInfo>;

//
// aliases for llvm::BasicBlock

template <typename EdgeInfoT>
using BasicBlockGenericDependenceNode =
    GenericDependenceNode<llvm::BasicBlock, GeneralInfo, EdgeInfoT>;

using BasicBlockDependenceNode =
    BasicBlockGenericDependenceNode<BasicDependenceInfo>;

template <typename EdgeInfoT>
using BasicBlockGenericDependenceGraph =
    GenericDependenceGraph<BasicBlockGenericDependenceNode<EdgeInfoT>>;

using BasicBlockDependenceGraph =
    BasicBlockGenericDependenceGraph<BasicDependenceInfo>;

} // namespace pedigree

// graph traits specializations

namespace llvm {

// node traits specialization meant to be used as a supplement to the graph
// traits specialization

// traits for llvm::Instruction aliases

template <>
struct GraphTraits<pedigree::InstructionDependenceNode *>
    : public pedigree::LLVMDependenceGraphNodeTraitsHelperBase<
          pedigree::InstructionDependenceNode *> {};

template <>
struct GraphTraits<const pedigree::InstructionDependenceNode *>
    : public pedigree::LLVMDependenceGraphNodeTraitsHelperBase<
          const pedigree::InstructionDependenceNode *> {};

template <>
struct GraphTraits<pedigree::InstructionDependenceGraph *>
    : public pedigree::LLVMDependenceGraphTraitsHelperBase<
          pedigree::InstructionDependenceGraph *> {};

template <>
struct GraphTraits<const pedigree::InstructionDependenceGraph *>
    : public pedigree::LLVMDependenceGraphTraitsHelperBase<
          const pedigree::InstructionDependenceGraph *> {};

// inverse traits for llvm::Instruction aliases

template <>
struct GraphTraits<Inverse<pedigree::InstructionDependenceNode *>>
    : public pedigree::LLVMDependenceInverseGraphNodeTraitsHelperBase<
          pedigree::InstructionDependenceNode *> {};

template <>
struct GraphTraits<Inverse<const pedigree::InstructionDependenceNode *>>
    : public pedigree::LLVMDependenceInverseGraphNodeTraitsHelperBase<
          const pedigree::InstructionDependenceNode *> {};

template <>
struct GraphTraits<Inverse<pedigree::InstructionDependenceGraph *>>
    : public pedigree::LLVMDependenceInverseGraphTraitsHelperBase<
          pedigree::InstructionDependenceGraph *> {};

template <>
struct GraphTraits<Inverse<const pedigree::InstructionDependenceGraph *>>
    : public pedigree::LLVMDependenceInverseGraphTraitsHelperBase<
          const pedigree::InstructionDependenceGraph *> {};

// DOT traits for llvm::Instruction aliases

template <>
struct DOTGraphTraits<pedigree::InstructionDependenceGraph *>
    : public pedigree::LLVMDOTDependenceGraphTraitsHelperBase<
          pedigree::InstructionDependenceGraph *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsHelperBase<
      pedigree::InstructionDependenceGraph *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}
};

// traits for llvm::BasicBlock aliases

template <>
struct GraphTraits<pedigree::BasicBlockDependenceNode *>
    : public pedigree::LLVMDependenceGraphNodeTraitsHelperBase<
          pedigree::BasicBlockDependenceNode *> {};

template <>
struct GraphTraits<const pedigree::BasicBlockDependenceNode *>
    : public pedigree::LLVMDependenceGraphNodeTraitsHelperBase<
          const pedigree::BasicBlockDependenceNode *> {};

template <>
struct GraphTraits<pedigree::BasicBlockDependenceGraph *>
    : public pedigree::LLVMDependenceGraphTraitsHelperBase<
          pedigree::BasicBlockDependenceGraph *> {};

template <>
struct GraphTraits<const pedigree::BasicBlockDependenceGraph *>
    : public pedigree::LLVMDependenceGraphTraitsHelperBase<
          const pedigree::BasicBlockDependenceGraph *> {};

// inverse traits for llvm::BasicBlock aliases

template <>
struct GraphTraits<Inverse<pedigree::BasicBlockDependenceNode *>>
    : public pedigree::LLVMDependenceInverseGraphNodeTraitsHelperBase<
          pedigree::BasicBlockDependenceNode *> {};

template <>
struct GraphTraits<Inverse<const pedigree::BasicBlockDependenceNode *>>
    : public pedigree::LLVMDependenceInverseGraphNodeTraitsHelperBase<
          const pedigree::BasicBlockDependenceNode *> {};

template <>
struct GraphTraits<Inverse<pedigree::BasicBlockDependenceGraph *>>
    : public pedigree::LLVMDependenceInverseGraphTraitsHelperBase<
          pedigree::BasicBlockDependenceGraph *> {};

template <>
struct GraphTraits<Inverse<const pedigree::BasicBlockDependenceGraph *>>
    : public pedigree::LLVMDependenceInverseGraphTraitsHelperBase<
          const pedigree::BasicBlockDependenceGraph *> {};

// DOT traits for llvm::BasicBlock aliases

template <>
struct DOTGraphTraits<pedigree::BasicBlockDependenceGraph *>
    : public pedigree::LLVMDOTDependenceGraphTraitsHelperBase<
          pedigree::BasicBlockDependenceGraph *> {
  using Base = pedigree::LLVMDOTDependenceGraphTraitsHelperBase<
      pedigree::BasicBlockDependenceGraph *>;

  DOTGraphTraits(bool isSimple = false) : Base(isSimple) {}
};

} // namespace llvm

#endif // header
