//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DDGraph.hpp"

#include "Pedigree/Analysis/Creational/DDGraphBuilder.hpp"

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

