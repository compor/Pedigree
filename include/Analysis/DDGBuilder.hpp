//
//
//

#ifndef PEDIGREE_DDGBUILDER_HPP
#define PEDIGREE_DDGBUILDER_HPP

#include "Config.hpp"

#include "DDG.hpp"

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

namespace pedigree {

class DDGBuilder : public llvm::InstVisitor<DDGBuilder> {
  DDG &m_Graph;

public:
  DDGBuilder(DDG &Graph) : m_Graph(Graph) {}

  template <typename T> void build(T &Unit) { visit(Unit); }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    BasicDependenceInfo info{};
    info.setOrigin(DependenceOrigin::data);
    // always flow for SSA use-def chains
    // info.setHazard(DependenceHazard::flow);

    auto src = m_Graph.getOrInsertNode(&CurInstruction);
    for (auto &u : CurInstruction.uses()) {
      auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
      if (user) {
        auto dst = m_Graph.getOrInsertNode(user);
        src->addDependentNode(dst, info);
      }
    }
  }
};

} // namespace pedigree end

#endif // header
