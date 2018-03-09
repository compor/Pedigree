//
//
//

#ifndef PEDIGREE_MDASIMPLEMDGBUILDER_HPP
#define PEDIGREE_MDASIMPLEMDGBUILDER_HPP

#include "Config.hpp"

#include "MDG.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis;

namespace pedigree {

class MDASimpleMDGBuilder : public llvm::InstVisitor<MDASimpleMDGBuilder> {
  MDG &m_Graph;
  llvm::MemoryDependenceAnalysis &m_MDA;

public:
  MDASimpleMDGBuilder(MDG &Graph, const llvm::MemoryDependenceAnalysis &MDA)
      : m_Graph(Graph),
        m_MDA(const_cast<llvm::MemoryDependenceAnalysis &>(MDA)) {}

  void visitInstruction(llvm::Instruction &CurInstruction) {
    // TODO maybe we should consider providing an option for not including nodes
    // in the graph unless they have an edge
    auto dst = m_Graph.getOrInsertNode(&CurInstruction);

    if (!CurInstruction.mayReadOrWriteMemory())
      return;

    auto result = m_MDA.getDependency(&CurInstruction);

    if (result.getInst() && result.isDef()) {
      auto src = m_Graph.getOrInsertNode(result.getInst());
      src->addDependentNode(dst);
    }
  }
};

} // namespace pedigree end

#endif // header
