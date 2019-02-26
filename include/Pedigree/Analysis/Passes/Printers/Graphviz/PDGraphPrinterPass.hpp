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

#define PEDIGREE_PDG_DOT_PASS_NAME "pedigree-pdg-dot"

namespace pedigree {

// new passmanager pass
class PDGraphDOTPrinterPass
    : public llvm::PassInfoMixin<PDGraphDOTPrinterPass> {

public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &FAM);
};

} // namespace pedigree
