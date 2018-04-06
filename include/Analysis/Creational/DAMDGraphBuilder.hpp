//
//
//

#ifndef PEDIGREE_DAMDGRAPHBUILDER_HPP
#define PEDIGREE_DAMDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "Analysis/Graphs/MDGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/Analysis/DependenceAnalysis.h"
// using llvm::DependenceAnalysis

#include "boost/optional.hpp"
// using boost::optional

#include <memory>
// using std::unique_ptr
// using std::make_unique

#include <vector>
// using std::vector

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

// TODO maybe we should consider providing an option for not including nodes
// in the graph unless they have an edge

class DAMDGraphBuilder : public llvm::InstVisitor<DAMDGraphBuilder> {
  std::unique_ptr<MDGraph> Graph;
  boost::optional<llvm::DependenceAnalysis &> CurAnalysis;
  boost::optional<const llvm::Function &> CurUnit;
  std::vector<llvm::Instruction *> MemInstructions;

  void visitMemRefInstruction(llvm::Instruction &CurInstruction) {
    MemInstructions.emplace_back(&CurInstruction);
  }

public:
  DAMDGraphBuilder() = default;

  DAMDGraphBuilder &setAnalysis(llvm::DependenceAnalysis &Analysis) {
    CurAnalysis = const_cast<llvm::DependenceAnalysis &>(Analysis);

    return *this;
  }

  DAMDGraphBuilder &setUnit(const llvm::Function &Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  std::unique_ptr<MDGraph> build() {
    if (CurUnit && CurAnalysis) {
      Graph = std::make_unique<MDGraph>();
      visit(const_cast<llvm::Function &>(*CurUnit));

      constexpr BasicDependenceInfo info{DependenceOrigin::Memory,
                                         DependenceHazard::Flow};

      for (auto ii = std::begin(MemInstructions),
                ie = std::end(MemInstructions);
           ii != ie; ++ii) {
        auto src = Graph->getOrInsertNode(*ii);

        for (auto jj = ii; jj != ie; ++jj)
          if (auto D = CurAnalysis->depends(*ii, *jj, true)) {
            auto dst = Graph->getOrInsertNode(*jj);
            src->addDependentNode(dst, info);
          }
      }
    }

    return std::move(Graph);
  }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory())
      visitMemRefInstruction(CurInstruction);
  }
};

} // namespace pedigree end

#endif // header
