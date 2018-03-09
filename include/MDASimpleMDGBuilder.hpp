//
//
//

#ifndef PEDIGREE_MDALOCALMDGBUILDER_HPP
#define PEDIGREE_MDALOCALMDGBUILDER_HPP

#include "Config.hpp"

#include "MDG.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/CallSite.h"
// using llvm::ImmutableCallSite

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis
// using llvm::MemDepResult
// using llvm::NonLocalDepResult

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include <cassert>
// using assert

namespace pedigree {

class MDALocalMDGBuilder : public llvm::InstVisitor<MDALocalMDGBuilder> {
  MDG &m_Graph;
  llvm::MemoryDependenceAnalysis &m_MDA;
  bool m_isBlockLocal;

public:
  MDALocalMDGBuilder(MDG &Graph, const llvm::MemoryDependenceAnalysis &MDA,
                     bool isBlockLocal = true)
      : m_Graph(Graph),
        m_MDA(const_cast<llvm::MemoryDependenceAnalysis &>(MDA)),
        m_isBlockLocal(isBlockLocal) {}

  template <typename T> void build(T &Unit) { visit(Unit); }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    // TODO maybe we should consider providing an option for not including nodes
    // in the graph unless they have an edge
    auto dst = m_Graph.getOrInsertNode(&CurInstruction);

    if (!CurInstruction.mayReadOrWriteMemory() &&
        !llvm::ImmutableCallSite(&CurInstruction))
      return;

    auto query = m_MDA.getDependency(&CurInstruction);
    assert(query.getInst() && "Dependee instruction is null!");

    llvm::SmallVector<llvm::Instruction *, 8> dependees;

    if (query.isNonLocal()) {
      llvm::SmallVector<llvm::NonLocalDepResult, 8> result;
      m_MDA.getNonLocalPointerDependency(&CurInstruction, result);

      for (const auto &e : result)
        dependees.push_back(e.getResult().getInst());
    } else {
      if (query.isDef())
        dependees.push_back(query.getInst());
    }

    for (const auto &e : dependees) {
      auto src = m_Graph.getOrInsertNode(e);
      src->addDependentNode(dst);
    }
  }
};

} // namespace pedigree end

#endif // header
