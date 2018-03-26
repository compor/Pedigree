//
//
//

#ifndef PEDIGREE_MDGRAPHPASS_HPP
#define PEDIGREE_MDGRAPHPASS_HPP

#include "Config.hpp"

#include "Analysis/MDGraph.hpp"

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

struct MDGraphPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<MDGraph> Graph;

  MDGraphPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const MDGraph &getGraph() const { return *Graph; }
  MDGraph &getGraph() { return *Graph; }
};

} // namespace pedigree end

#endif // header
