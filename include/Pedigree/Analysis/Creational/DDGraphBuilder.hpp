//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DDGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/IntrinsicInst.h"
// using llvm::IntrinsicInst

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/ADT/Optional.h"
// using llvm::Optional

#include "llvm/ADT/SmallPtrSet.h"
// using llvm::SmallPtrSet

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <memory>
// using std::unique_ptr
// using std::make_unique

#define DEBUG_TYPE "pedigree-ddg-builder"

namespace llvm {
class Function;
} // namespace llvm

namespace pedigree {

class DDGraphBuilder : public llvm::InstVisitor<DDGraphBuilder> {
  DDGraphResultT Graph;
  llvm::Optional<const llvm::Function *> CurUnit;
  bool shouldIgnoreConstantPHINodes = false;

  llvm::SmallPtrSet<llvm::Instruction *, 32> SkipSet;

  bool shouldSkip(llvm::IntrinsicInst &CurI) {
    auto intrID = CurI.getIntrinsicID();

    switch (intrID) {
    default:
    case llvm::Intrinsic::lifetime_start:
    case llvm::Intrinsic::lifetime_end:
    case llvm::Intrinsic::invariant_start:
    case llvm::Intrinsic::invariant_end:
    case llvm::Intrinsic::launder_invariant_group:
    case llvm::Intrinsic::strip_invariant_group:
      return true;
    }

    return false;
  }

  bool shouldSkip(llvm::Instruction *CurI) {
    assert(CurI && "Pointer is null!");

    if (SkipSet.count(CurI)) {
      return true;
    }

    bool status = false;

    if (auto *ii = llvm::dyn_cast<llvm::IntrinsicInst>(CurI)) {
      status |= shouldSkip(*ii);
    }

    if (status) {
      SkipSet.insert(CurI);
    }

    return status;
  }

  bool shouldSkip(llvm::Instruction &CurI) { return shouldSkip(&CurI); }

public:
  DDGraphBuilder() = default;

  DDGraphBuilder &setUnit(const llvm::Function *Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  DDGraphBuilder &setUnit(const llvm::Function &Unit) { return setUnit(&Unit); }

  DDGraphBuilder &ignoreConstantPHINodes(bool Val) {
    shouldIgnoreConstantPHINodes = Val;

    return *this;
  }

  DDGraphResultT build() {
    if (CurUnit) {
      Graph = std::make_unique<DDGraph>();
      visit(const_cast<llvm::Function *>(*CurUnit));
    }

    return std::move(Graph);
  }

  void visitInstruction(llvm::Instruction &CurI) {
    if (shouldSkip(CurI)) {
      return;
    }

    for (auto &u : CurI.uses()) {
      auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
      if (user && !shouldSkip(user)) {
        auto src = Graph->getOrInsertNode(&CurI);
        auto dst = Graph->getOrInsertNode(user);
        src->addDependentNode(dst, {DO_Data, DH_Flow});
      }
    }
  }

  void visitPHINode(llvm::PHINode &CurI) {
    if (shouldSkip(CurI)) {
      return;
    }

    if (!shouldIgnoreConstantPHINodes) {
      for (auto &e : CurI.incoming_values()) {
        if (llvm::isa<llvm::Constant>(e.get())) {
          auto dst = Graph->getOrInsertNode(&CurI);
          auto src =
              Graph->getOrInsertNode(CurI.getIncomingBlock(e)->getTerminator());
          src->addDependentNode(dst, {DO_Data, DH_Flow});
        }
      }
    }

    visitInstruction(CurI);
  }
};

} // namespace pedigree

#undef DEBUG_TYPE

