//
//
//

#ifndef PEDIGREE_DAMDGBUILDER_HPP
#define PEDIGREE_DAMDGBUILDER_HPP

#include "Config.hpp"

#include "MDG.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/CallSite.h"
// using llvm::ImmutableCallSite

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include <cassert>
// using assert

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class DAMDGBuilder : public llvm::InstVisitor<DAMDGBuilder> {
  MDG &m_Graph;
  llvm::DependenceAnalysis &m_DA;

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {}

public:
  DAMDGBuilder(MDG &Graph, const llvm::DependenceAnalysis &DA)
      : m_Graph(Graph), m_DA(const_cast<llvm::DependenceAnalysis &>(DA)) {}

  template <typename T> void build(T &Unit) { visit(Unit); }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory() &&
        !llvm::ImmutableCallSite(&CurInstruction))
      visitMemRefInstruction(CurInstruction);
  }
};

} // namespace pedigree end

#endif // header
