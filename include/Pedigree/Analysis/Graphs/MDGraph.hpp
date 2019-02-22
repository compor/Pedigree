//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/DependenceGraphs.hpp"

#include <memory>
// using std::unique_ptr

namespace pedigree {

using MemoryDependenceNode = InstructionDependenceNode;
using MDGraph = InstructionDependenceGraph;

using MDGraphResultT = std::unique_ptr<MDGraph>;

} // namespace pedigree

