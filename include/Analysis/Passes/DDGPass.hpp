//
//
//

#ifndef PEDIGREE_DDGPASS_HPP
#define PEDIGREE_DDGPASS_HPP

#include "Config.hpp"

#include "Analysis/DDG.hpp"

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

struct DDGPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<DDG> Graph;

  DDGPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const DDG &getGraph() const { return *Graph; }
  DDG &getGraph() { return *Graph; }
};

} // namespace pedigree end

#endif // header
