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

#include <cassert>
// using assert

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm

#define PEDIGREE_PDG_PASS_NAME "pedigree-pdg"

namespace pedigree {

// new passmanager pass
class PDGraphPass : public llvm::AnalysisInfoMixin<PDGraphPass> {
  friend llvm::AnalysisInfoMixin<PDGraphPass>;

  static llvm::AnalysisKey Key;

public:
  using Result = PDGraphResultT;

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

