//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/MDGraph.hpp"

#include "llvm/Pass.h"
// using llvm::FunctionPass
// using llvm::AnalysisUsage
// using llvm::RegisterPass

#include "llvm/IR/PassManager.h"
// using llvm::FunctionAnalysisManager
// using llvm::AnalysisInfoMixin
// using llvm::PreservedAnalyses

#include <cassert>
// using assert

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm

#define PEDIGREE_MDG_PASS_NAME "pedigree-mdg"

namespace pedigree {

// new passmanager pass
class MDGraphAnalysis : public llvm::AnalysisInfoMixin<MDGraphAnalysis> {
  friend llvm::AnalysisInfoMixin<MDGraphAnalysis>;

  static llvm::AnalysisKey Key;

public:
  using Result = MDGraphResultT;

  bool invalidate(llvm::Function &F, const llvm::PreservedAnalyses &PA,
                  llvm::FunctionAnalysisManager::Invalidator &Inv);

  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
};

// legacy passmanager pass
struct MDGraphWrapperPass : public llvm::FunctionPass {
  static char ID;
  MDGraphResultT Graph;

  MDGraphWrapperPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const MDGraph &getGraph() const {
    assert(Graph && "Graph is null!");
    return *Graph;
  }

  MDGraph &getGraph() {
    assert(Graph && "Graph is null!");
    return *Graph;
  }
};

} // namespace pedigree

