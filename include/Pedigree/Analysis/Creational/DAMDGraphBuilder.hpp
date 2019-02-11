//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/MDGraph.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

#include "llvm/ADT/Optional.h"
// using llvm::Optional

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <vector>
// using std::vector

#define DEBUG_TYPE "pedigree-mdg-da-builder"

namespace llvm {
class Function;
} // namespace llvm

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class DAMDGraphBuilder : public llvm::InstVisitor<DAMDGraphBuilder> {
  std::unique_ptr<MDGraph> Graph;
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  llvm::Optional<llvm::DependenceAnalysis *> CurAnalysis;
#else
  llvm::Optional<llvm::DependenceInfo *> CurAnalysis;
#endif
  llvm::Optional<const llvm::Function *> CurUnit;
  std::vector<llvm::Instruction *> MemInstructions;

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    MemInstructions.emplace_back(&CurInstruction);
  }

public:
  DAMDGraphBuilder() = default;

#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  DAMDGraphBuilder &setAnalysis(llvm::DependenceAnalysis &Analysis) {
    CurAnalysis = const_cast<llvm::DependenceAnalysis *>(&Analysis);

    return *this;
  }
#else
  DAMDGraphBuilder &setAnalysis(llvm::DependenceInfo &Analysis) {
    CurAnalysis = const_cast<llvm::DependenceInfo *>(&Analysis);

    return *this;
  }
#endif

  DAMDGraphBuilder &setUnit(const llvm::Function *Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  DAMDGraphBuilder &setUnit(const llvm::Function &Unit) {
    return setUnit(&Unit);
  }

  std::unique_ptr<MDGraph> build() {
    if (CurUnit && CurAnalysis) {
      Graph = std::make_unique<MDGraph>();
      visit(const_cast<llvm::Function *>(*CurUnit));

      for (auto ii = std::begin(MemInstructions),
                ie = std::end(MemInstructions);
           ii != ie; ++ii) {
        auto src = Graph->getOrInsertNode(*ii);

        for (auto jj = ii; jj != ie; ++jj) {
          if (auto D = (*CurAnalysis)->depends(*ii, *jj, true)) {
            auto dst = Graph->getOrInsertNode(*jj);
            src->addDependentNode(dst, {DO_Memory, DH_Flow});
          }
        }
      }
    }

    return std::move(Graph);
  }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory()) {
      visitMemRefInstruction(CurInstruction);
    }
  }
};

} // namespace pedigree

