//
//
//

#ifndef PEDIGREE_MDALOCALMDGRAPHBUILDER_HPP
#define PEDIGREE_MDALOCALMDGRAPHBUILDER_HPP

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Analysis/Graphs/MDGraph.hpp"

#include "Pedigree/Analysis/AnalysisScope.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

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

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include "boost/optional.hpp"
// using boost::optional

#include "flags/flags.hpp"
// using ALLOW_FLAGS_FOR_ENUM

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <cassert>
// using assert

#define DEBUG_TYPE "pedigree-mdg-mda-builder"

namespace llvm {
class Function;
} // namespace llvm

namespace pedigree {
enum class AnalysisMode : uint8_t;
} // namespace pedigree

ALLOW_FLAGS_FOR_ENUM(pedigree::AnalysisMode);

namespace pedigree {

enum class AnalysisMode : uint8_t {
  Unknown = 0,
  MemDefs = 0b01,
  MemClobbers = 0b10,
};

class MDAMDGraphBuilder : public llvm::InstVisitor<MDAMDGraphBuilder> {
private:
  std::unique_ptr<MDGraph> Graph;
  boost::optional<const llvm::Function &> CurUnit;
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  boost::optional<llvm::MemoryDependenceAnalysis &> CurAnalysis;
#else
  boost::optional<llvm::MemoryDependenceResults &> CurAnalysis;
#endif

  AnalysisScope CurScope;
  flags::flags<AnalysisMode> CurMode;

public:
  MDAMDGraphBuilder()
      : CurScope(AnalysisScope::Block), CurMode(AnalysisMode::MemDefs) {}

#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  MDAMDGraphBuilder &setAnalysis(llvm::MemoryDependenceAnalysis &Analysis) {
    CurAnalysis = const_cast<llvm::MemoryDependenceAnalysis &>(Analysis);

    return *this;
  }
#else
  MDAMDGraphBuilder &setAnalysis(llvm::MemoryDependenceResults &Analysis) {
    CurAnalysis = const_cast<llvm::MemoryDependenceResults &>(Analysis);

    return *this;
  }
#endif

  MDAMDGraphBuilder &setUnit(const llvm::Function &Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  MDAMDGraphBuilder &setScope(AnalysisScope Scope) {
    CurScope = Scope;

    return *this;
  }

  MDAMDGraphBuilder &turnOnMode(AnalysisMode Mode) {
    CurMode |= Mode;

    return *this;
  }

  std::unique_ptr<MDGraph> build() {
    assert(!CurMode.empty() && "Analysis mode is empty!");

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

    if (query.isUnknown()) {
      return;
    }

    if (query.isNonFuncLocal()) {
      if (CurScope >= AnalysisScope::Interprocedural) {
        getInterproceduralDependees(llvm::CallSite(&CurInstruction));
      }
    } else if (query.isNonLocal()) {
      if (CurScope >= AnalysisScope::Function) {
        getFunctionLocalDependees(CurInstruction);
      }
    } else {
      // AnalysisScope::Block is the minimum and thus always on
      getBlockLocalDependees(query, CurInstruction);
    }
  }

  decltype(auto) determineHazard(const llvm::Instruction &Src,
                                 const llvm::Instruction &Dst,
                                 const llvm::MemDepResult &QueryResult) {
    BasicDependenceInfo::value_type info{DependenceOrigin::Memory,
                                         DependenceHazard::Unknown};

    if (QueryResult.isDef() && (CurMode & AnalysisMode::MemDefs)) {
      if (Src.mayReadFromMemory() && Dst.mayReadFromMemory()) {
        // do not add edge
      } else if (Src.mayReadFromMemory() && Dst.mayWriteToMemory()) {
        info.hazards |= DependenceHazard::Anti;
      } else if (Src.mayWriteToMemory() && Dst.mayReadFromMemory()) {
        info.hazards |= DependenceHazard::Flow;
      } else if (Src.mayWriteToMemory() && Dst.mayWriteToMemory()) {
        info.hazards |= DependenceHazard::Out;
      } else {
        LLVM_DEBUG(llvm::dbgs() << "No appropriate hazard was found!");
      }
    }

    if (QueryResult.isClobber() && (CurMode & AnalysisMode::MemClobbers)) {
      if (Dst.mayReadFromMemory()) {
        info.hazards |= DependenceHazard::Flow;
      } else if (Dst.mayWriteToMemory()) {
        info.hazards |= DependenceHazard::Out;
      } else {
        LLVM_DEBUG(llvm::dbgs() << "No appropriate hazard was found!");
      }
    }

    return info;
  }

  void addDependenceWithInfo(llvm::Instruction &Src, llvm::Instruction &Dst,
                             const BasicDependenceInfo::value_type &Info) {
    auto src = Graph->getOrInsertNode(&Src);
    auto dst = Graph->getOrInsertNode(&Dst);
    src->addDependentNode(dst, Info);
  }

  void getInterproceduralDependees(llvm::CallSite Dst) {
    if (!Dst) {
      return;
    }

    auto &results = CurAnalysis->getNonLocalCallDependency(Dst);
    auto dst = Dst.getInstruction();

    for (auto &e : results) {
      auto &queryResult = e.getResult();
      auto *src = queryResult.getInst();
      auto info = determineHazard(*src, *dst, queryResult);

      if (info.hazards) {
        addDependenceWithInfo(*src, *dst, info);
      }
    }
  }

  void getFunctionLocalDependees(llvm::Instruction &Dst) {
    llvm::SmallVector<llvm::NonLocalDepResult, 8> results;
    CurAnalysis->getNonLocalPointerDependency(&Dst, results);

    for (const auto &e : results) {
      auto &queryResult = e.getResult();
      auto *src = queryResult.getInst();
      auto info = determineHazard(*src, Dst, queryResult);

      if (info.hazards) {
        addDependenceWithInfo(*src, Dst, info);
      }
    }
  }

  void getBlockLocalDependees(llvm::MemDepResult &QueryResult,
                              llvm::Instruction &Dst) {
    auto *src = QueryResult.getInst();
    auto info = determineHazard(*src, Dst, QueryResult);

    if (info.hazards) {
      addDependenceWithInfo(*src, Dst, info);
    }
  }
};

} // namespace pedigree

#endif // header
