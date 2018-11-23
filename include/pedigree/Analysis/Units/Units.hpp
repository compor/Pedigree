//
//
//

#ifndef PEDIGREE_UNITS_UNITS_HPP
#define PEDIGREE_UNITS_UNITS_HPP

#include "pedigree/Config.hpp"

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

#endif // header
