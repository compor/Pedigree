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

  void visitInstruction(llvm::Instruction &CurInstruction) {
    auto src = m_Graph.getOrInsertNode(&CurInstruction);
    for (auto &u : CurInstruction.uses()) {
      auto *user = llvm::dyn_cast<llvm::Instruction>(u.getUser());
      if (user) {
        auto dst = m_Graph.getOrInsertNode(user);
        src->addDependentNode(dst);
      }
    }
  }
};

} // namespace pedigree end

#endif // header
