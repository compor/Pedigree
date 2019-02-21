//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "Pedigree/Analysis/Graphs/MDGraph.hpp"

#include "Pedigree/Analysis/AnalysisScope.hpp"

#include "Pedigree/Support/Traits/TypeTraits.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/CallSite.h"
// using llvm::CallSite

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
// using llvm::MemoryDependenceAnalysis
// using llvm::MemDepResult
// using llvm::NonLocalDepResult

#include "llvm/ADT/SmallVector.h"
// using llvm::SmallVector

#include "llvm/ADT/Optional.h"
// using llvm::Optional

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include <memory>
// using std::unique_ptr
// using std::make_unique

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
  std::unique_ptr<MDGraph> Graph;
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
  std::unique_ptr<MDGraph> build(IteratorT Begin, IteratorT End) {
    assert(!CurMode.none() && "Analysis mode is empty!");

    if (CurAnalysis) {
      Graph = std::make_unique<MDGraph>();

      for (auto it = Begin; it != End; ++it) {
        processMemInstruction(*it);
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

    if (QueryResult.isDef() && CurMode[MDA_MD_MemDefs]) {
      if (Src.mayReadFromMemory() && Dst.mayReadFromMemory()) {
        // do not add edge
      } else if (Src.mayReadFromMemory() && Dst.mayWriteToMemory()) {
        info |= {DO_Memory, DH_Anti};
      } else if (Src.mayWriteToMemory() && Dst.mayReadFromMemory()) {
        info |= {DO_Memory, DH_Flow};
      } else if (Src.mayWriteToMemory() && Dst.mayWriteToMemory()) {
        info |= {DO_Memory, DH_Out};
      } else {
        LLVM_DEBUG(llvm::dbgs() << "No appropriate hazard was found!");
      }
    }

    if (QueryResult.isClobber() && CurMode[MDA_MD_MemClobbers]) {
      if (Dst.mayReadFromMemory()) {
        info |= {DO_Memory, DH_Flow};
      } else if (Dst.mayWriteToMemory()) {
        info |= {DO_Memory, DH_Out};
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
};

} // namespace pedigree

#undef DEBUG_TYPE

