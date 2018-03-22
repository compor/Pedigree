//
//
//

#ifndef PEDIGREE_UNITADAPTOS_HPP
#define PEDIGREE_UNITADAPTOS_HPP

#include "Config.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace llvm {
class Instruction;
}

namespace pedigree {

struct BlockToInstructionUnitAdaptor {
  llvm::Instruction *operator()(const llvm::BasicBlock *From) {
    auto *from = const_cast<llvm::BasicBlock *>(From);
    return from->getTerminator();
  }
};

} // namespace pedigree end

#endif // header
