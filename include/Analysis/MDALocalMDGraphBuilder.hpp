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

#include <cassert>
// using assert

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class MDALocalMDGraphBuilder
    : public llvm::InstVisitor<MDALocalMDGraphBuilder> {
public:
  MDALocalMDGraphBuilder(MDGraph &Graph,
                         const llvm::MemoryDependenceAnalysis &MDA)
      : Graph(Graph), MDA(const_cast<llvm::MemoryDependenceAnalysis &>(MDA)),
        Scope(AnalysisScope::Block) {}

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
  AnalysisScope Scope;

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    auto query = MDA.getDependency(&CurInstruction);

    if (query.isNonLocal()) {
      if (Scope >= AnalysisScope::Interprocedural)
        getInterproceduralDependees(llvm::CallSite(&CurInstruction));
      else if (Scope >= AnalysisScope::Function)
        getFunctionLocalDependees(CurInstruction);
    } else {
      // AnalysisScope::Block is the minimum and thus always on
      getBlockLocalDependees(query, CurInstruction);
    }
  }

  decltype(auto) determineHazard(const llvm::Instruction &Src,
                                 const llvm::Instruction &Dst,
                                 const llvm::MemDepResult &QueryResult) {
    BasicDependenceInfo info{DependenceOrigin::Memory,
                             DependenceHazard::Unknown};

    // TODO decide what to do when the query is unknown
    if (QueryResult.isUnknown())
      return info;

    if (QueryResult.isDef()) {
      if (Src.mayReadFromMemory() && Dst.mayReadFromMemory())
        // TODO decide how to handle these cases (if any) better
        assert(true && "A RAW hazard was reported!");

      if (Src.mayReadFromMemory() && Dst.mayWriteToMemory())
        info.hazards |= DependenceHazard::Anti;

      if (Src.mayWriteToMemory() && Dst.mayReadFromMemory())
        info.hazards |= DependenceHazard::Flow;

      if (Src.mayWriteToMemory() && Dst.mayWriteToMemory())
        info.hazards |= DependenceHazard::Out;
    } else if (QueryResult.isClobber()) {
      if (Dst.mayReadFromMemory())
        info.hazards |= DependenceHazard::Flow;

      if (Dst.mayWriteToMemory())
        info.hazards |= DependenceHazard::Out;
    }

    return info;
  }

  void addDependenceWithInfo(llvm::Instruction &Src, llvm::Instruction &Dst,
                             const BasicDependenceInfo &Info) {
    auto src = Graph.getOrInsertNode(&Src);
    auto dst = Graph.getOrInsertNode(&Dst);
    src->addDependentNode(dst, Info);
  }

  void getInterproceduralDependees(llvm::CallSite Dst) {
    if (!Dst)
      return;

    auto &results = MDA.getNonLocalCallDependency(Dst);
    auto dst = Dst.getInstruction();

    for (auto &e : results) {
      auto &result = e.getResult();
      auto src = result.getInst();
      auto info = determineHazard(*src, *dst, result);

      addDependenceWithInfo(*src, *dst, info);
    }
  }

  void getFunctionLocalDependees(llvm::Instruction &Dst) {
    llvm::SmallVector<llvm::NonLocalDepResult, 8> result;
    MDA.getNonLocalPointerDependency(&Dst, result);

    for (const auto &e : result) {
      auto &result = e.getResult();
      auto src = result.getInst();
      auto info = determineHazard(*src, Dst, result);

      addDependenceWithInfo(*src, Dst, info);
    }
  }

  void getBlockLocalDependees(llvm::MemDepResult &Query,
                              llvm::Instruction &Dst) {
    auto src = Query.getInst();
    auto info = determineHazard(*src, Dst, Query);

    addDependenceWithInfo(*src, Dst, info);
  }
};

} // namespace pedigree end

#endif // header
