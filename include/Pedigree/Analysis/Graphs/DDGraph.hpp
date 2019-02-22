//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include <memory>
// using std::unique_ptr

namespace pedigree {

using DataDependenceNode = InstructionDependenceNode;
using DDGraph = InstructionDependenceGraph;

using DDGraphResultT = std::unique_ptr<DDGraph>;

} // namespace pedigree

