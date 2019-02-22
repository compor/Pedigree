//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DDGraph.hpp"

#include "llvm/Pass.h"
// using llvm::FunctionPass
// using llvm::AnalysisUsage
// using llvm::RegisterPass

#include "llvm/IR/PassManager.h"
// using llvm::FunctionAnalysisManager
// using llvm::AnalysisInfoMixin

#include <cassert>
// using assert

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm

#define PEDIGREE_DDG_PASS_NAME "pedigree-ddg"

namespace pedigree {

// new passmanager pass
class DDGraphAnalysis : public llvm::AnalysisInfoMixin<DDGraphAnalysis> {
  friend llvm::AnalysisInfoMixin<DDGraphAnalysis>;

  static llvm::AnalysisKey Key;

public:
  using Result = DDGraphResultT;

  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
};

// legacy passmanager pass
struct DDGraphWrapperPass : public llvm::FunctionPass {
  static char ID;
  DDGraphResultT Graph;

  DDGraphWrapperPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const DDGraph &getGraph() const {
    assert(Graph && "Graph is null!");
    return *Graph;
  }

  DDGraph &getGraph() {
    assert(Graph && "Graph is null!");
    return *Graph;
  }
};

} // namespace pedigree

