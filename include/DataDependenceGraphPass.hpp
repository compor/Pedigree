//
//
//

#ifndef DATADEPENDENCEGRAPHPASS_HPP
#define DATADEPENDENCEGRAPHPASS_HPP

#include "Config.hpp"

#include "llvm/Pass.h"
// using llvm::FunctionPass
// using llvm::AnalysisUsage
// using llvm::RegisterPass

#include "DataDependenceGraph.hpp"

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm end

namespace pedigree {

struct DataDependenceGraphPass : public llvm::FunctionPass {
  static char ID;

  DataDependenceGraphPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;
};

} // namespace pedigree end

#endif // header
