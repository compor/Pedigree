//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/PDGraph.hpp"

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

#define PEDIGREE_PDG_PASS_NAME "pedigree-pdg"

namespace pedigree {

// new passmanager pass
class PDGraphAnalysis : public llvm::AnalysisInfoMixin<PDGraphAnalysis> {
  friend llvm::AnalysisInfoMixin<PDGraphAnalysis>;

  static llvm::AnalysisKey Key;

public:
  using Result = PDGraphResultT;

  bool invalidate(llvm::Function &F, const llvm::PreservedAnalyses &PA,
                  llvm::FunctionAnalysisManager::Invalidator &Inv);

  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
};

// legacy passmanager pass
struct PDGraphWrapperPass : public llvm::FunctionPass {
  static char ID;
  PDGraphResultT Graph;

  PDGraphWrapperPass();

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const PDGraph &getGraph() const {
    assert(Graph && "Graph is null!");
    return *Graph;
  }

  PDGraph &getGraph() {
    assert(Graph && "Graph is null!");
    return *Graph;
  }
};

} // namespace pedigree

