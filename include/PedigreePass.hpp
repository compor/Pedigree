//
//
//

#ifndef PEDIGREEPASS_HPP
#define PEDIGREEPASS_HPP

#include "llvm/Pass.h"
// using llvm::FunctionPass

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

struct PedigreePass : public llvm::FunctionPass {
  static char ID;

  PedigreePass() : llvm::FunctionPass(ID) {}

  bool runOnFunction(llvm::Function &CurFunc) override;
};

} // namespace pedigree end

#endif // header
