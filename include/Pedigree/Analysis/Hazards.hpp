//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Info/EdgeInfo/BasicDependenceInfo.hpp"

namespace llvm {
class Instruction;
}

namespace pedigree {

struct BasicDependenceInfo;

BasicDependenceInfo::value_type DetermineHazard(const llvm::Instruction *Src,
                                                const llvm::Instruction *Dst);

BasicDependenceInfo::value_type DetermineHazard(const llvm::Instruction &Src,
                                                const llvm::Instruction &Dst);

} // namespace pedigree
