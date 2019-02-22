//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/CDGraph.hpp"

#include "Pedigree/Analysis/Creational/CDGraphBuilder.hpp"

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

#define PEDIGREE_CDG_PASS_NAME "pedigree-cdg"

namespace pedigree {

// new passmanager pass
class CDGraphPass : public llvm::AnalysisInfoMixin<CDGraphPass> {
  friend llvm::AnalysisInfoMixin<CDGraphPass>;

  static llvm::AnalysisKey Key;

public:
  using Result = CDGraphResultT;

  Result run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM);
};

// legacy passmanager pass
struct CDGraphWrapperPass : public llvm::FunctionPass {
  static char ID;
  CDGraphResultT Graph;
  std::unique_ptr<InstCDGraph> InstGraph;

  CDGraphWrapperPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const CDGraph &getGraph() const {
    assert(Graph && "Graph is null!");
    return *Graph;
  }

  CDGraph &getGraph() {
    assert(Graph && "Graph is null!");
    return *Graph;
  }

  const InstCDGraph &getInstGraph() const {
    assert(InstGraph && "Graph is null!");
    return *InstGraph;
  }

  InstCDGraph &getInstGraph() {
    assert(InstGraph && "Graph is null!");
    return *InstGraph;
  }
};

} // namespace pedigree

