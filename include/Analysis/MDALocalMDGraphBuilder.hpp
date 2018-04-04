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
                         const llvm::MemoryDependenceAnalysis &MDA)
      : Graph(Graph), MDA(const_cast<llvm::MemoryDependenceAnalysis &>(MDA)),
        dst(nullptr), Scope(AnalysisScope::Block) {}

  MDALocalMDGraphBuilder &setScope(AnalysisScope AS) {
    Scope = AS;
    return *this;
  }

  template <typename T> void build(T &Unit) { visit(Unit); }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory())
      visitMemRefInstruction(CurInstruction);
  }

private:
  MDGraph &Graph;
  llvm::MemoryDependenceAnalysis &MDA;
  llvm::Instruction *dst;
  AnalysisScope Scope;

  void getInterproceduralDependees(
      llvm::CallSite CS,
      llvm::SmallVectorImpl<llvm::Instruction *> &Dependees) {
    if (!CS)
      return;

    auto &result = MDA.getNonLocalCallDependency(CS);

    for (auto &e : result)
      Dependees.emplace_back(e.getResult().getInst());
  }

  void getFunctionLocalDependees(
      llvm::Instruction &CurInstruction,
      llvm::SmallVectorImpl<llvm::Instruction *> &Dependees) {
    llvm::SmallVector<llvm::NonLocalDepResult, 8> result;
    MDA.getNonLocalPointerDependency(&CurInstruction, result);

    for (const auto &e : result)
      Dependees.emplace_back(e.getResult().getInst());
  }

  void getBlockLocalDependees(
      llvm::MemDepResult &Query, llvm::Instruction &CurInstruction,
      llvm::SmallVectorImpl<llvm::Instruction *> &Dependees) {
    // TODO what about clobbers?
    if (Query.isDef())
      Dependees.emplace_back(Query.getInst());
  }

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    auto query = MDA.getDependency(&CurInstruction);
    auto dst = Graph.getOrInsertNode(&CurInstruction);
    llvm::SmallVector<llvm::Instruction *, 8> dependees;

    if (query.isNonLocal()) {
      if (Scope >= AnalysisScope::Interprocedural)
        getInterproceduralDependees(llvm::CallSite(&CurInstruction), dependees);
      else if (Scope >= AnalysisScope::Function)
        getFunctionLocalDependees(CurInstruction, dependees);
    } else {
      // AnalysisScope::Block is the minimum and thus always on
      getBlockLocalDependees(query, CurInstruction, dependees);
    }

    constexpr BasicDependenceInfo info{DependenceOrigin::Memory,
                                       DependenceHazard::Flow};

    for (const auto &e : dependees) {
      auto src = Graph.getOrInsertNode(e);
      src->addDependentNode(dst, info);
    }
  }
};

} // namespace pedigree end

#endif // header
