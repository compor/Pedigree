//
//
//

#ifndef PEDIGREE_CDGRAPHBUILDER_HPP
#define PEDIGREE_CDGRAPHBUILDER_HPP

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Graphs/CDGraph.hpp"

#include "Pedigree/Analysis/PostDominanceFrontier.hpp"

#include "llvm/Config/llvm-config.h"
// version macros

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

#include "llvm/Support/Debug.h"
// using LLVM_DEBUG macro
// using llvm::dbgs

#include "boost/optional.hpp"
// using boost::optional

#include <memory>
// using std::unique_ptr
// using std::make_unique

#define DEBUG_TYPE "pedigree-cdg-builder"

namespace llvm {
class Function;
} // namespace llvm

namespace pedigree {

class CDGraphBuilder {
  boost::optional<const llvm::Function &> CurUnit;

  static constexpr BasicDependenceInfo::value_type info{
      DependenceOrigin::Control, DependenceHazard::Unknown};

public:
  CDGraphBuilder() = default;

  CDGraphBuilder &setUnit(const llvm::Function &Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  std::unique_ptr<CDGraph> build() {
    if (!CurUnit) {
      return {};
    }

    llvm::PostDominatorTree curPDT;
    PostDominanceFrontierBase<llvm::BasicBlock> pdf;

#if (LLVM_VERSION_MAJOR >= 5)
    curPDT.recalculate(const_cast<llvm::Function &>(*CurUnit));
    pdf.analyze(curPDT);
#else
    curPDT.DT->recalculate(const_cast<llvm::Function &>(*CurUnit));
    pdf.analyze(*curPDT.DT);
#endif

    auto Graph = std::make_unique<CDGraph>();

    for (auto &f : pdf) {
      for (auto &e : f.second) {
        auto dst = Graph->getOrInsertNode(f.first);
        auto src = Graph->getOrInsertNode(e);
        src->addDependentNode(dst, info);
      }
    }

    return Graph;
  }
};

constexpr BasicDependenceInfo::value_type CDGraphBuilder::info;

} // namespace pedigree

#endif // header
