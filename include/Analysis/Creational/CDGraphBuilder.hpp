//
//
//

#ifndef PEDIGREE_CDGRAPHBUILDER_HPP
#define PEDIGREE_CDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "Analysis/Graphs/CDGraph.hpp"

#include "Analysis/PostDominanceFrontier.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

#include "boost/optional.hpp"
// using boost::optional

#include <memory>
// using std::unique_ptr
// using std::make_unique

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

class CDGraphBuilder {
  boost::optional<const llvm::Function &> CurUnit;

  static constexpr BasicDependenceInfo info{DependenceOrigin::Control,
                                            DependenceHazard::Unknown};

public:
  CDGraphBuilder() = default;

  CDGraphBuilder &setUnit(const llvm::Function &Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  std::unique_ptr<CDGraph> build() {
    if (!CurUnit)
      return {};

    llvm::PostDominatorTree curPDT;
    curPDT.DT->recalculate(const_cast<llvm::Function &>(*CurUnit));

    PostDominanceFrontierBase<llvm::BasicBlock> pdf;
    pdf.analyze(*curPDT.DT);

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

constexpr BasicDependenceInfo CDGraphBuilder::info;

} // namespace pedigree end

#endif // header
