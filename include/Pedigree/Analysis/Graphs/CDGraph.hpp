//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include <memory>
// using std::unique_ptr

namespace pedigree {

using ControlDependenceNode = BasicBlockDependenceNode;
using CDGraph = BasicBlockDependenceGraph;

using InstControlDependenceNode = InstructionDependenceNode;
using InstCDGraph = InstructionDependenceGraph;

using CDGraphResultT = std::unique_ptr<CDGraph>;

} // namespace pedigree

