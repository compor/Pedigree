//
//
//

#ifndef PEDIGREE_UNITCONVERTERS_HPP
#define PEDIGREE_UNITCONVERTERS_HPP

#include "Config.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/IR/InstrTypes.h"
// using llvm::TerminatorInst

namespace llvm {
class Instruction;
}

namespace pedigree {

struct BlockToInstructionUnitConverter {
  llvm::Instruction *operator()(const llvm::BasicBlock *From) {
    auto *from = const_cast<llvm::BasicBlock *>(From);
    return from->getTerminator();
  }
};

} // namespace pedigree end

#endif // header
