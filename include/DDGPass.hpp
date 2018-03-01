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

#include <memory>
// using std::unique_ptr

namespace llvm {
class Function;
class AnalysisUsage;
} // namespace llvm end

namespace pedigree {

struct DDGPass : public llvm::FunctionPass {
  static char ID;
  std::unique_ptr<DataDependenceGraph> m_Graph;

  DDGPass() : llvm::FunctionPass(ID) {}

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
  bool runOnFunction(llvm::Function &CurFunction) override;

  const DataDependenceGraph &getGraph() const { return *m_Graph; }
  DataDependenceGraph &getGraph() {
    return const_cast<DataDependenceGraph &>(
        static_cast<const DDGPass *>(this)->getGraph());
  }
};

} // namespace pedigree end

#endif // header
