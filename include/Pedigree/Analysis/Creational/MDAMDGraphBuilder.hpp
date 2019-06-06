//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Analysis/Graphs/MDGraph.hpp"

#include "Pedigree/Analysis/AnalysisScope.hpp"

#include "Pedigree/Analysis/Hazards.hpp"

#include "Pedigree/Support/Traits/TypeTraits.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/IntrinsicInst.h"
// using llvm::IntrinsicInst

#include "llvm/IR/CallSite.h"
// using llvm::CallSite

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis
// using llvm::MemDepResult
// using llvm::NonLocalDepResult

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include "llvm/ADT/SmallPtrSet.h"
// using llvm::SmallPtrSet

#include "llvm/ADT/Optional.h"
// using llvm::Optional

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <bitset>
// using std::bitset

#include <cassert>
// using assert

#define DEBUG_TYPE "pedigree-mdg-mda-builder"

namespace llvm {
class Function;
} // namespace llvm

namespace pedigree {

enum MDA_MD_AnalysisMode : uint8_t {
  MDA_MD_MemDefs,
  MDA_MD_MemClobbers,
  MDA_MD_COUNT
};

class MDAMDGraphBuilder {
private:
  MDGraphResultT Graph;
#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  llvm::Optional<llvm::MemoryDependenceAnalysis *> CurAnalysis;
#else
  llvm::Optional<llvm::MemoryDependenceResults *> CurAnalysis;
#endif

  AnalysisScope CurScope;
  std::bitset<MDA_MD_COUNT> CurMode;

public:
  MDAMDGraphBuilder() : CurScope(AnalysisScope::Block) {
    CurMode[MDA_MD_MemDefs] = true;
  }

#if (LLVM_VERSION_MAJOR <= 3 && LLVM_VERSION_MINOR < 9)
  MDAMDGraphBuilder &setAnalysis(llvm::MemoryDependenceAnalysis &Analysis) {
    CurAnalysis = const_cast<llvm::MemoryDependenceAnalysis *>(&Analysis);

    return *this;
  }
#else
  MDAMDGraphBuilder &setAnalysis(llvm::MemoryDependenceResults &Analysis) {
    CurAnalysis = const_cast<llvm::MemoryDependenceResults *>(&Analysis);

    return *this;
  }
#endif

  MDAMDGraphBuilder &setScope(AnalysisScope Scope) {
    CurScope = Scope;

    return *this;
  }

  MDAMDGraphBuilder &turnOnMode(MDA_MD_AnalysisMode Mode) {
    CurMode[Mode] = true;

    return *this;
  }

  template <typename IteratorT>
  MDGraphResultT build(IteratorT Begin, IteratorT End) {
    assert(!CurMode.none() && "Analysis mode is empty!");

    if (CurAnalysis) {
      Graph = std::make_unique<MDGraph>();

      for (auto it = Begin; it != End; ++it) {
        if (!shouldSkip(*it)) {
          processMemInstruction(*it);
        }
      }
    }

    return std::move(Graph);
  }

private:
  void processMemInstruction(llvm::Instruction &CurInstruction) {
    auto query = (*CurAnalysis)->getDependency(&CurInstruction);

    if (query.isUnknown()) {
      return;
    }

    if (query.isNonFuncLocal()) {
      if (CurScope >= AnalysisScope::Interprocedural) {
        getInterproceduralDependees(llvm::CallSite(&CurInstruction));
      }
    } else if (query.isNonLocal()) {
      if (CurScope >= AnalysisScope::Function) {
        getIntraproceduralDependees(CurInstruction);
      }
    } else {
      // AnalysisScope::Block is the minimum and thus always on
      getIntrablockDependees(query, CurInstruction);
    }
  }

  void processMemInstruction(llvm::Instruction *CurInstruction) {
    assert(CurInstruction && "Instruction is null!");

    processMemInstruction(*CurInstruction);
  }

  decltype(auto) determineHazard(const llvm::Instruction &Src,
                                 const llvm::Instruction &Dst,
                                 const llvm::MemDepResult &QueryResult) {
    BasicDependenceInfo::value_type info;
    LLVM_DEBUG(llvm::dbgs() << "in " << __func__ << "\n\tsrc: " << Src
                            << "\n\tdst: " << Dst << '\n';);

    if (QueryResult.isDef() && CurMode[MDA_MD_MemDefs]) {
      info |= DetermineHazard(Src, Dst);
    }

    if (QueryResult.isClobber() && CurMode[MDA_MD_MemClobbers]) {
      info |= DetermineHazard(Src, Dst);
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

    auto &results = (*CurAnalysis)->getNonLocalCallDependency(Dst);
    auto dst = Dst.getInstruction();

    for (auto &e : results) {
      auto &queryResult = e.getResult();
      auto *src = queryResult.getInst();
      auto info = determineHazard(*src, *dst, queryResult);

      if (info) {
        addDependenceWithInfo(*src, *dst, info);
      }
    }
  }

  void getIntraproceduralDependees(llvm::Instruction &Dst) {
    llvm::SmallVector<llvm::NonLocalDepResult, 8> results;
    (*CurAnalysis)->getNonLocalPointerDependency(&Dst, results);

    for (const auto &e : results) {
      auto &queryResult = e.getResult();
      auto *src = queryResult.getInst();
      auto info = determineHazard(*src, Dst, queryResult);

      if (info) {
        addDependenceWithInfo(*src, Dst, info);
      }
    }
  }

  void getIntrablockDependees(llvm::MemDepResult &QueryResult,
                              llvm::Instruction &Dst) {
    auto *src = QueryResult.getInst();
    auto info = determineHazard(*src, Dst, QueryResult);

    if (info) {
      addDependenceWithInfo(*src, Dst, info);
    }
  }

  llvm::SmallPtrSet<llvm::Instruction *, 32> SkipSet;

  bool shouldSkip(llvm::IntrinsicInst &CurI) {
    auto intrID = CurI.getIntrinsicID();

    switch (intrID) {
    default:
    case llvm::Intrinsic::lifetime_start:
    case llvm::Intrinsic::lifetime_end:
    case llvm::Intrinsic::invariant_start:
    case llvm::Intrinsic::invariant_end:
    case llvm::Intrinsic::launder_invariant_group:
    case llvm::Intrinsic::strip_invariant_group:
      return true;
    }

    return false;
  }

  bool shouldSkip(llvm::Instruction *CurI) {
    assert(CurI && "Pointer is null!");

    if (SkipSet.count(CurI)) {
      return true;
    }

    bool status = false;

    if (auto *ii = llvm::dyn_cast<llvm::IntrinsicInst>(CurI)) {
      status |= shouldSkip(*ii);
    }

    if (status) {
      SkipSet.insert(CurI);
    }

    return status;
  }

  bool shouldSkip(llvm::Instruction &CurI) { return shouldSkip(&CurI); }
};

} // namespace pedigree

#undef DEBUG_TYPE
