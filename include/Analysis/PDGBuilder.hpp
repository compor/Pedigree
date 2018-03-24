//
//
//

#ifndef PEDIGREE_PDGBUILDER_HPP
#define PEDIGREE_PDGBUILDER_HPP

#include "Config.hpp"

#include "PDG.hpp"

#include "DDG.hpp"

#include "CDG.hpp"

#include "MDG.hpp"

namespace pedigree {

class PDGBuilder {
  const CDG &cdg;
  const DDG &ddg;
  const MDG &mdg;

public:
  PDGBuilder(const CDG &c, const DDG &d, const MDG &m)
      : cdg(c), ddg(d), mdg(m) {}

  void build(PDG &G) const {
    for (auto &e : ddg) {
      auto src = G.getOrInsertNode(e.first);

      for (auto &k : *e.second) {
        auto dst = G.getOrInsertNode(k.first->get());
        src->addDependentNode(dst, {});
      }
    }
  }
};

} // namespace pedigree end

#endif // header
