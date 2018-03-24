//
//
//

#ifndef PEDIGREE_DDGraphPASS_HPP
#define PEDIGREE_DDGraphPASS_HPP

#include "Config.hpp"

#include "Analysis/DDGraph.hpp"

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

struct DDGraphPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<DDGraph> Graph;

  DDGraphPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const DDGraph &getGraph() const { return *Graph; }
  DDGraph &getGraph() { return *Graph; }
};

} // namespace pedigree end

#endif // header
