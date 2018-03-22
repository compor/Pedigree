//
//
//

#ifndef PEDIGREE_CDGBUILDER_HPP
#define PEDIGREE_CDGBUILDER_HPP

#include "Config.hpp"

#include "CDG.hpp"

#include "PostDominanceFrontier.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include "llvm/Analysis/PostDominators.h"
// using llvm::PostDominatorTree

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

class CDGBuilder {
  CDG &m_Graph;

public:
  CDGBuilder(CDG &Graph) : m_Graph(Graph) {}

  void build(const llvm::Function &CurFunc) {
    llvm::PostDominatorTree curPDT;
    curPDT.DT->recalculate(const_cast<llvm::Function &>(CurFunc));

    PostDominanceFrontierBase<llvm::BasicBlock> pdf;
    pdf.analyze(*curPDT.DT);

    BasicDependenceInfo info{};
    info.setOrigin(DependenceOrigin::control);

    for (auto &f : pdf) {
      auto dst = m_Graph.getOrInsertNode(f.first);
      for (auto &e : f.second) {
        auto src = m_Graph.getOrInsertNode(e);
        src->addDependentNode(dst, info);
      }
    }
  }
};

} // namespace pedigree end

#endif // header
