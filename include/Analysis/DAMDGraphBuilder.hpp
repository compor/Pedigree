//
//
//

#ifndef PEDIGREE_DAMDGRAPHBUILDER_HPP
#define PEDIGREE_DAMDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "MDGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

#include <vector>
// using std::vector

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class DAMDGraphBuilder : public llvm::InstVisitor<DAMDGraphBuilder> {
  MDGraph &Graph;
  llvm::DependenceAnalysis &m_DA;
  std::vector<llvm::Instruction *> m_MemInstructions;

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    m_MemInstructions.push_back(&CurInstruction);
  }

public:
  DAMDGraphBuilder(MDGraph &Graph, const llvm::DependenceAnalysis &DA)
      : Graph(Graph), m_DA(const_cast<llvm::DependenceAnalysis &>(DA)) {}

  template <typename T> void build(T &Unit) {
    visit(Unit);

    BasicDependenceInfo info{};
    info.setOrigin(DependenceOrigin::Memory);
    // TODO
    // info.setHazard();

    for (auto ii = std::begin(m_MemInstructions),
              ie = std::end(m_MemInstructions);
         ii != ie; ++ii) {
      auto src = Graph.getOrInsertNode(*ii);

      for (auto jj = ii; jj != ie; ++jj)
        if (auto D = m_DA.depends(*ii, *jj, true)) {
          auto dst = Graph.getOrInsertNode(*jj);
          src->addDependentNode(dst, info);
        }
    }
  }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory())
      visitMemRefInstruction(CurInstruction);
  }
};

} // namespace pedigree end

#endif // header
