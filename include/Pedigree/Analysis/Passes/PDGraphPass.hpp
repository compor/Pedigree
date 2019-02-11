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

#include <memory>
// using std::unique_ptr

#include <cassert>
// using assert

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm

namespace pedigree {

struct PDGraphPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<PDGraph> Graph;

  PDGraphPass();

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

