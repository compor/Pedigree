//
//
//

#ifndef TESTCOMMON_HPP
#define TESTCOMMON_HPP

#include "llvm/Config/llvm-config.h"

#include "llvm/IR/Function.h"
// using llvm::Function

#include "llvm/IR/Dominators.h"
// using llvm::DominatorTree

#include "llvm/Analysis/LoopInfo.h"
// using llvm::LoopInfo

#include <iostream>
// using std::ostream

#include <string>
// using std::string

namespace pedigree {
namespace testing {

static llvm::LoopInfo calculateLoopInfo(llvm::Function &Func) {
  llvm::DominatorTree DT;
  llvm::LoopInfo LI;

  DT.recalculate(Func);
#if (LLVM_VERSION_MAJOR >= 4) ||                                               \
    (LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR >= 8)
  LI.analyze(DT);
#else
  LI.Analyze(DT);
#endif

  return LI;
}

struct ExampleTestData {
  std::string assemblyFile;
};

std::ostream &operator<<(std::ostream &os, const ExampleTestData &td) {
  auto delim = ' ';
  return os << delim << "assembly file: " << td.assemblyFile << delim;
}

} // namespace testing end
} // namespace pedigree end

#endif // header
