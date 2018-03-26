//
//
//

#ifndef PEDIGREE_CDGRAPH_HPP
#define PEDIGREE_CDGRAPH_HPP

#include "Config.hpp"

#include "DependenceGraphs.hpp"

namespace pedigree {

using ControlDependenceNode = BasicBlockDependenceNode;
using CDGraph = BasicBlockDependenceGraph;

using InstControlDependenceNode = InstructionDependenceNode;
using InstCDGraph = InstructionDependenceGraph;

} // namespace pedigree end

#endif // header
