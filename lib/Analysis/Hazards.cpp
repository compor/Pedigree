//
//
//

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Analysis/Hazards.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#define DEBUG_TYPE "pdg-hazards"

namespace pedigree {

BasicDependenceInfo::value_type DetermineHazard(const llvm::Instruction &Src,
                                                const llvm::Instruction &Dst) {
  BasicDependenceInfo::value_type info;
  LLVM_DEBUG(llvm::dbgs() << "in " << __func__ << "\n\tsrc: " << Src
                          << "\n\tdst: " << Dst << '\n';);

  if (Src.mayReadFromMemory() && Dst.mayReadFromMemory()) {
    // do not add edge
  }

  if (Src.mayReadFromMemory() && Dst.mayWriteToMemory()) {
    LLVM_DEBUG(llvm::dbgs() << "adding def anti\n";);
    info |= {DO_Memory, DH_Anti};
  }

  if (Src.mayWriteToMemory() && Dst.mayReadFromMemory()) {
    LLVM_DEBUG(llvm::dbgs() << "adding def flow\n";);
    info |= {DO_Memory, DH_Flow};
  }

  if (Src.mayWriteToMemory() && Dst.mayWriteToMemory()) {
    LLVM_DEBUG(llvm::dbgs() << "adding def out\n";);
    info |= {DO_Memory, DH_Out};
  }

  return info;
}

} // namespace pedigree
