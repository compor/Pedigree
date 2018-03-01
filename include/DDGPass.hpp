//
//
//

#ifndef DDGPASS_HPP
#define DDGPASS_HPP

#include "Config.hpp"

#include "DDG.hpp"

#include "DDGBuilder.hpp"

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
  std::unique_ptr<DDG> m_Graph;

  DDGPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const DDG &getGraph() const { return *m_Graph; }
  DDG &getGraph() { return *m_Graph; }
};

} // namespace pedigree end

#endif // header
