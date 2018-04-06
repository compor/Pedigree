//
//
//

#ifndef PEDIGREE_DDGRAPHBUILDER_HPP
#define PEDIGREE_DDGRAPHBUILDER_HPP

#include "Config.hpp"

#include "Analysis/Graphs/DDGraph.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "boost/optional.hpp"
// using boost::optional

#include <memory>
// using std::unique_ptr
// using std::make_unique

namespace llvm {
class Function;
} // namespace llvm end

namespace pedigree {

class DDGraphBuilder : public llvm::InstVisitor<DDGraphBuilder> {
  std::unique_ptr<DDGraph> Graph;
  boost::optional<const llvm::Function &> CurUnit;

  // always flow for SSA use-def chains
  static constexpr BasicDependenceInfo info{DependenceOrigin::Data,
                                            DependenceHazard::Flow};

public:
  DDGraphBuilder() = default;

  DDGraphBuilder &setUnit(const llvm::Function &Unit) {
    CurUnit.emplace(Unit);

    return *this;
  }

  std::unique_ptr<DDGraph> build() {
    if (CurUnit) {
      Graph = std::make_unique<DDGraph>();
      visit(const_cast<llvm::Function &>(*CurUnit));
    }

    return std::move(Graph);
  }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    for (auto &u : CurInstruction.uses()) {
      auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
      if (user) {
        auto src = Graph->getOrInsertNode(&CurInstruction);
        auto dst = Graph->getOrInsertNode(user);
        src->addDependentNode(dst, info);
      }
    }
  }
};

constexpr BasicDependenceInfo DDGraphBuilder::info;

} // namespace pedigree end

#endif // header