//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Util.hpp"

#include "llvm/IR/PassManager.h"
// using llvm::FunctionAnalysisManager
// using llvm::PassInfoMixin
// using llvm::PreservedAnalyses

namespace llvm {
class Function;
} // namespace llvm

#define PEDIGREE_DDG_DOT_PASS_NAME "pedigree-ddg-dot"

namespace pedigree {

// new passmanager pass
class DDGraphDOTPrinterPass
    : public llvm::PassInfoMixin<DDGraphDOTPrinterPass> {

public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM);
};

} // namespace pedigree
