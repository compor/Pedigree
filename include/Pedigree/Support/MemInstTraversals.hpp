//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "Pedigree/Debug.hpp"

#include "llvm/IR/InstVisitor.h"
// using llvm::InstVisitor

#include "llvm/IR/Instruction.h"
// using llvm::Instruction

#include <iterator>
// using std::begin
// using std::end

namespace pedigree {

class NaiveMemInstTraversal : public llvm::InstVisitor<NaiveMemInstTraversal> {
  using ContainerT = std::vector<llvm::Instruction *>;
  ContainerT MemInstructions;

public:
  template <typename UnitT> explicit NaiveMemInstTraversal(UnitT &Unit) {
    using std::begin;
    using std::end;
    visit(begin(Unit), end(Unit));
  }

  template <typename UnitT> explicit NaiveMemInstTraversal(UnitT *Unit) {
    visit(*Unit);
  }

  template <typename IteratorT>
  NaiveMemInstTraversal(IteratorT Start, IteratorT End) {
    visit(Start, End);
  }

  void visitInstruction(llvm::Instruction &CurInstruction) {
    if (CurInstruction.mayReadOrWriteMemory()) {
      MemInstructions.emplace_back(&CurInstruction);
    }
  }

  using iterator = ContainerT::iterator;
  using const_iterator = ContainerT::const_iterator;

  decltype(auto) begin() { return MemInstructions.begin(); }
  decltype(auto) end() { return MemInstructions.end(); }

  decltype(auto) begin() const { return MemInstructions.begin(); }
  decltype(auto) end() const { return MemInstructions.end(); }
};

} // namespace pedigree

