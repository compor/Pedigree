//
//
//

#ifndef PEDIGREE_PDGRAPHPASS_HPP
#define PEDIGREE_PDGRAPHPASS_HPP

#include "Config.hpp"

#include "Analysis/Graphs/PDGraph.hpp"

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

struct PDGraphPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<PDGraph> Graph;

  PDGraphPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const PDGraph &getGraph() const { return *Graph; }
  PDGraph &getGraph() { return *Graph; }
};

} // namespace pedigree end

#endif // header
