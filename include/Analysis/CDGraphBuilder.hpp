//
//
//

#ifndef PEDIGREE_CDGRAPHBUILDER_HPP
#define PEDIGREE_CDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "CDGraph.hpp"

#include "PostDominanceFrontier.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

class CDGraphBuilder {
  CDGraph &Graph;

public:
  CDGraphBuilder(CDGraph &Graph) : Graph(Graph) {}

  void build(const llvm::Function &CurFunc) {
    llvm::PostDominatorTree curPDT;
    curPDT.DT->recalculate(const_cast<llvm::Function &>(CurFunc));

    PostDominanceFrontierBase<llvm::BasicBlock> pdf;
    pdf.analyze(*curPDT.DT);

    BasicDependenceInfo info{};
    info.setOrigin(DependenceOrigin::Control);

    for (auto &f : pdf) {
      auto dst = Graph.getOrInsertNode(f.first);
      for (auto &e : f.second) {
        auto src = Graph.getOrInsertNode(e);
        src->addDependentNode(dst, info);
      }
    }
  }
};

} // namespace pedigree end

#endif // header
