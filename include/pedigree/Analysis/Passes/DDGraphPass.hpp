//
//
//

#ifndef PEDIGREE_DDGRAPHPASS_HPP
#define PEDIGREE_DDGRAPHPASS_HPP

#include "pedigree/Config.hpp"

#include "pedigree/Analysis/Graphs/DDGraph.hpp"

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

struct DDGraphPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<DDGraph> Graph;

  DDGraphPass() : llvm::FunctionPass(ID) {}

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

#endif // header
