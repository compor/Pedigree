//
//
//

#ifndef PEDIGREE_PDGPASS_HPP
#define PEDIGREE_PDGPASS_HPP

#include "Config.hpp"

#include "Analysis/PDG.hpp"

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

struct PDGPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<PDG> Graph;

  PDGPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const PDG &getGraph() const { return *Graph; }
  PDG &getGraph() { return *Graph; }
};

} // namespace pedigree end

#endif // header
