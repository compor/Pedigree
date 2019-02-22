//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include <memory>
// using std::unique_ptr

namespace pedigree {

using ProgramDependenceNode = InstructionDependenceNode;
using PDGraph = InstructionDependenceGraph;

using PDGraphResultT = std::unique_ptr<PDGraph>;

} // namespace pedigree

