//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

namespace pedigree {

using ControlDependenceNode = BasicBlockDependenceNode;
using CDGraph = BasicBlockDependenceGraph;

using InstControlDependenceNode = InstructionDependenceNode;
using InstCDGraph = InstructionDependenceGraph;

} // namespace pedigree

