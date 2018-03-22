//
//
//

#ifndef PEDIGREE_PDGBUILDER_HPP
#define PEDIGREE_PDGBUILDER_HPP

#include "Config.hpp"

#include "PDG.hpp"

namespace pedigree {

class PDGBuilder {
  PDG &m_Graph;

public:
  PDGBuilder(PDG &Graph) : m_Graph(Graph) {}

  // TODO fix builders interface
  void build() { BasicDependenceInfo info{}; }
};

} // namespace pedigree end

#endif // header
