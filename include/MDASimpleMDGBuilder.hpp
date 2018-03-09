//
//
//

#ifndef PEDIGREE_MDALOCALMDGBUILDER_HPP
#define PEDIGREE_MDALOCALMDGBUILDER_HPP

#include "Config.hpp"

#include "MDG.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis
// using llvm::MemDepResult

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

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

    if (!CurInstruction.mayReadOrWriteMemory())
      return;

    auto result = m_MDA.getDependency(&CurInstruction);

    if (!result.isNonLocal()) {
      if (result.getInst() && result.isDef()) {
        auto src = m_Graph.getOrInsertNode(result.getInst());
        src->addDependentNode(dst);
      }
    } else {
      llvm::SmallVector<llvm::NonLocalDepResult, 8> result;
      m_MDA.getNonLocalPointerDependency(&CurInstruction, result);

      for (auto e : result) {
        auto &res = e.getResult();
        auto src = m_Graph.getOrInsertNode(res.getInst());
        src->addDependentNode(dst);
      }
    }
  }
};

} // namespace pedigree end

#endif // header
