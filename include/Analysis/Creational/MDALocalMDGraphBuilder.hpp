//
//
//

#ifndef PEDIGREE_MDALOCALMDGRAPHBUILDER_HPP
#define PEDIGREE_MDALOCALMDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "Analysis/Graphs/MDGraph.hpp"

#include "Analysis/AnalysisScope.hpp"

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

#include "boost/optional.hpp"
// using boost::optional

#include "flags/flags.hpp"
// using ALLOW_FLAGS_FOR_ENUM

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <cassert>
// using assert

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

enum class AnalysisMode : uint8_t {
  Unknown = 0,
  MemDefs = 0b01,
  MemClobbers = 0b10,
};

} // namespace pedigree end

ALLOW_FLAGS_FOR_ENUM(pedigree::AnalysisMode);

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class MDALocalMDGraphBuilder
    : public llvm::InstVisitor<MDALocalMDGraphBuilder> {
private:
  std::unique_ptr<MDGraph> Graph;
  boost::optional<const llvm::Function &> CurUnit;
  boost::optional<llvm::MemoryDependenceAnalysis &> CurAnalysis;
  AnalysisScope CurScope;
  flags::flags<AnalysisMode> CurMode;

public:
  MDALocalMDGraphBuilder()
      : CurScope(AnalysisScope::Block), CurMode(AnalysisMode::MemDefs) {}

  MDALocalMDGraphBuilder &
  setAnalysis(llvm::MemoryDependenceAnalysis &Analysis) {
    CurAnalysis = const_cast<llvm::MemoryDependenceAnalysis &>(Analysis);

    return *this;
  }

  MDALocalMDGraphBuilder &setUnit(const llvm::Function &Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  MDALocalMDGraphBuilder &setScope(AnalysisScope Scope) {
    CurScope = Scope;

    return *this;
  }

  MDALocalMDGraphBuilder &setMode(AnalysisMode Mode) {
    CurMode |= Mode;

    return *this;
  }

  std::unique_ptr<MDGraph> build() {
    if (CurUnit && CurAnalysis) {
      Graph = std::make_unique<MDGraph>();
      visit(const_cast<llvm::Function &>(*CurUnit));
    }

    return std::move(Graph);
  }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory())
      visitMemRefInstruction(CurInstruction);
  }

private:
  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    auto query = CurAnalysis->getDependency(&CurInstruction);

    if (query.isNonLocal()) {
      if (CurScope >= AnalysisScope::Interprocedural)
        getInterproceduralDependees(llvm::CallSite(&CurInstruction));
      else if (CurScope >= AnalysisScope::Function)
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
    if (QueryResult.isUnknown() || CurMode.empty())
      return info;

    if (QueryResult.isDef() && (CurMode & AnalysisMode::MemDefs)) {
      if (Src.mayReadFromMemory() && Dst.mayReadFromMemory())
        // TODO decide how to handle these cases (if any) better
        assert(false && "A RAW hazard was reported!");

      if (Src.mayReadFromMemory() && Dst.mayWriteToMemory())
        info.hazards |= DependenceHazard::Anti;

      if (Src.mayWriteToMemory() && Dst.mayReadFromMemory())
        info.hazards |= DependenceHazard::Flow;

      if (Src.mayWriteToMemory() && Dst.mayWriteToMemory())
        info.hazards |= DependenceHazard::Out;
    } else if (QueryResult.isClobber() &&
               (CurMode & AnalysisMode::MemClobbers)) {
      if (Dst.mayReadFromMemory())
        info.hazards |= DependenceHazard::Flow;

      if (Dst.mayWriteToMemory())
        info.hazards |= DependenceHazard::Out;
    }

    return info;
  }

  void addDependenceWithInfo(llvm::Instruction &Src, llvm::Instruction &Dst,
                             const BasicDependenceInfo &Info) {
    auto src = Graph->getOrInsertNode(&Src);
    auto dst = Graph->getOrInsertNode(&Dst);
    src->addDependentNode(dst, Info);
  }

  void getInterproceduralDependees(llvm::CallSite Dst) {
    if (!Dst)
      return;

    auto &results = CurAnalysis->getNonLocalCallDependency(Dst);
    auto dst = Dst.getInstruction();

    for (auto &e : results) {
      auto &queryResult = e.getResult();
      auto src = queryResult.getInst();
      auto info = determineHazard(*src, *dst, queryResult);

      addDependenceWithInfo(*src, *dst, info);
    }
  }

  void getFunctionLocalDependees(llvm::Instruction &Dst) {
    llvm::SmallVector<llvm::NonLocalDepResult, 8> results;
    CurAnalysis->getNonLocalPointerDependency(&Dst, results);

    for (const auto &e : results) {
      auto &queryResult = e.getResult();
      auto src = queryResult.getInst();
      auto info = determineHazard(*src, Dst, queryResult);

      addDependenceWithInfo(*src, Dst, info);
    }
  }

  void getBlockLocalDependees(llvm::MemDepResult &QueryResult,
                              llvm::Instruction &Dst) {
    auto src = QueryResult.getInst();
    auto info = determineHazard(*src, Dst, QueryResult);

    addDependenceWithInfo(*src, Dst, info);
  }
};

} // namespace pedigree end

#endif // header
