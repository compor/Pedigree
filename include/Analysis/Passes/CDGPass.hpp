//
//
//

#ifndef PEDIGREE_CDGPASS_HPP
#define PEDIGREE_CDGPASS_HPP

#include "Config.hpp"

#include "Analysis/CDG.hpp"

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

struct CDGPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<CDG> Graph;
  std::unique_ptr<InstCDG> m_InstGraph;

  CDGPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const CDG &getGraph() const { return *Graph; }
  CDG &getGraph() { return *Graph; }

  const InstCDG &getInstGraph() const { return *m_InstGraph; }
  InstCDG &getInstGraph() { return *m_InstGraph; }
};

} // namespace pedigree end

#endif // header
