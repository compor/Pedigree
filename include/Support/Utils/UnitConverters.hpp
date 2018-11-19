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

#include "llvm/ADT/STLExtras.h"
// using llvm::make_filter_range

namespace llvm {
class Instruction;
}

namespace pedigree {

struct BlockToTerminatorUnitConverter {
  decltype(auto) operator()(const llvm::BasicBlock &From) {
    return llvm::make_filter_range(
        From, [](auto &e) { return llvm::isa<llvm::TerminatorInst>(e); });
  }

  decltype(auto) operator()(const llvm::BasicBlock *From) {
    return llvm::make_filter_range(
        *From, [](auto &e) { return llvm::isa<llvm::TerminatorInst>(e); });
  }
};

} // namespace pedigree

#endif // header
