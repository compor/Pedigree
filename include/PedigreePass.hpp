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

class PedigreePass : public llvm::FunctionPass {
public:
  static char ID;

  Pedigree() : llvm::FunctionPass(ID) {}

  bool runOnFunction(llvm::Function &CurFunc) override;
};

} // namespace pedigree end

#endif // header
