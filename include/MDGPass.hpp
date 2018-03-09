//
//
//

#ifndef PEDIGREE_MDGPASS_HPP
#define PEDIGREE_MDGPASS_HPP

#include "Config.hpp"

#include "MDG.hpp"

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

struct MDGPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<MDG> m_Graph;

  MDGPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const MDG &getGraph() const { return *m_Graph; }
  MDG &getGraph() { return *m_Graph; }
};

} // namespace pedigree end

#endif // header
