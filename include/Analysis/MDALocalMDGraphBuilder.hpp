//
//
//

#ifndef PEDIGREE_MDALOCALMDGRAPHBUILDER_HPP
#define PEDIGREE_MDALOCALMDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "MDGraph.hpp"

#include "AnalysisScope.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/CallSite.h"
// using llvm::CallSite

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis
// using llvm::MemDepResult
// using llvm::NonLocalDepResult

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class MDALocalMDGraphBuilder
    : public llvm::InstVisitor<MDALocalMDGraphBuilder> {
public:
  MDALocalMDGraphBuilder(MDGraph &Graph,
                         const llvm::MemoryDependenceAnalysis &MDA,
                         AnalysisScope scope = AnalysisScope::Block)
      : Graph(Graph), m_MDA(const_cast<llvm::MemoryDependenceAnalysis &>(MDA)),
        scope(scope) {}

  template <typename T> void build(T &Unit) { visit(Unit); }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory())
      visitMemRefInstruction(CurInstruction);
  }

private:
  MDGraph &Graph;
  llvm::MemoryDependenceAnalysis &m_MDA;
  AnalysisScope scope;

  void getInterproceduralDependees(
      llvm::CallSite CS,
      llvm::SmallVectorImpl<llvm::Instruction *> &Dependees) {
    if (!CS)
      return;

    auto &result = m_MDA.getNonLocalCallDependency(CS);

    for (auto &e : result)
      Dependees.push_back(e.getResult().getInst());
  }

  void getFunctionLocalDependees(
      llvm::Instruction &CurInstruction,
      llvm::SmallVectorImpl<llvm::Instruction *> &Dependees) {
    llvm::SmallVector<llvm::NonLocalDepResult, 8> result;
    m_MDA.getNonLocalPointerDependency(&CurInstruction, result);

    for (const auto &e : result)
      Dependees.push_back(e.getResult().getInst());
  }

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    auto query = m_MDA.getDependency(&CurInstruction);
    auto dst = Graph.getOrInsertNode(&CurInstruction);
    llvm::SmallVector<llvm::Instruction *, 8> dependees;

    if (query.isNonLocal()) {
      if (scope >= AnalysisScope::Interprocedural)
        getInterproceduralDependees(llvm::CallSite(&CurInstruction), dependees);
      else if (scope >= AnalysisScope::Function)
        getFunctionLocalDependees(CurInstruction, dependees);
    } else {
      // AnalysisScope::Block is the minimum and thus always on
      // TODO what about clobbers?
      if (query.isDef())
        dependees.push_back(query.getInst());
    }

    BasicDependenceInfo info{};
    info.origins |= DependenceOrigin::Memory;
    // TODO
    // info.setHazard();

    for (const auto &e : dependees) {
      auto src = Graph.getOrInsertNode(e);
      src->addDependentNode(dst, info);
    }
  }
};

} // namespace pedigree end

#endif // header
