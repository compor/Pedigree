//
//
//

#ifndef PEDIGREE_CDGRAPHPASS_HPP
#define PEDIGREE_CDGRAPHPASS_HPP

#include "Config.hpp"

#include "Analysis/CDGraph.hpp"

#include "llvm/Pass.h"
// using llvm::FunctionPass
// using llvm::AnalysisUsage
// using llvm::RegisterPass

#include <memory>
// using std::unique_ptr

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm end

namespace pedigree {

struct CDGraphPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<CDGraph> Graph;
  std::unique_ptr<InstCDGraph> InstGraph;

  CDGraphPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const CDGraph &getGraph() const { return *Graph; }
  CDGraph &getGraph() { return *Graph; }

  const InstCDGraph &getInstGraph() const { return *InstGraph; }
  InstCDGraph &getInstGraph() { return *InstGraph; }
};

} // namespace pedigree end

#endif // header
