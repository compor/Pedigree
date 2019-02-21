//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/CDGraph.hpp"

#include "llvm/Pass.h"
// using llvm::FunctionPass
// using llvm::AnalysisUsage
// using llvm::RegisterPass

#include <memory>
// using std::unique_ptr

#include <cassert>
// using assert

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm

namespace pedigree {

struct CDGraphWrapperPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<CDGraph> Graph;
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

