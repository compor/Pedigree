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

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

namespace pedigree {

class PDGBuilder {
  const CDG &cdg;
  const DDG &ddg;
  const MDG &mdg;

public:
  PDGBuilder(const CDG &c, const DDG &d, const MDG &m)
      : cdg(c), ddg(d), mdg(m) {}

  void build(PDG &G) const {
    using PDGTraits = llvm::GraphTraits<PDG *>;

    for (const auto &e : llvm::make_range(PDGTraits::nodes_begin(&G),
                                          PDGTraits::nodes_end(&G))) {
      auto src = G.getOrInsertNode(e->get());

      for (const auto &k : llvm::make_range(PDGTraits::child_begin(e),
                                            PDGTraits::child_end(e))) {
        auto dst = G.getOrInsertNode(k->get());
        src->addDependentNode(dst, {});
      }
    }
  }
};

} // namespace pedigree end

#endif // header
