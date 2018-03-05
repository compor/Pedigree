//
//
//

#ifndef PEDIGREE_CDGBUILDER_HPP
#define PEDIGREE_CDGBUILDER_HPP

#include "Config.hpp"

#include "CDG.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

namespace pedigree {

class CDGBuilder {
  CDG &m_Graph;

public:
  CDGBuilder(CDG &Graph) : m_Graph(Graph) {}
};

} // namespace pedigree end

#endif // header
