//
//
//

#pragma once

#include "Pedigree/Config.hpp"

namespace llvm {
class Instruction;
class BasicBlock;
class Function;
} // namespace llvm

namespace pedigree {

struct instruction_unit_tag {};
struct basicblock_unit_tag {};
struct function_unit_tag {};

} // namespace pedigree

