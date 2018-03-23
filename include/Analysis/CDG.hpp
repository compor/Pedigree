//
//
//

#ifndef PEDIGREE_CDG_HPP
#define PEDIGREE_CDG_HPP

#include "Config.hpp"

#include "DependenceGraphs.hpp"

namespace pedigree {

using ControlDependenceNode = BasicBlockDependenceNode;
using CDG = BasicBlockDependenceGraph;

using InstControlDependenceNode = InstructionDependenceNode;
using InstCDG = InstructionDependenceGraph;

} // namespace pedigree end

#endif // header
