//
//
//

#ifndef PEDIGREE_PDGraphBUILDER_HPP
#define PEDIGREE_PDGraphBUILDER_HPP

#include "Config.hpp"

#include "PDGraph.hpp"

#include "DDGraph.hpp"

#include "CDGraph.hpp"

#include "MDGraph.hpp"

#include "llvm/ADT/iterator_range.h"
// using llvm::make_range

namespace pedigree {

class PDGraphBuilder {
  const CDGraph &cdg;
  const DDGraph &ddg;
  const MDGraph &mdg;

public:
  PDGraphBuilder(const CDGraph &c, const DDGraph &d, const MDGraph &m)
      : cdg(c), ddg(d), mdg(m) {}

  void build(PDGraph &G) const {
    using PDGraphTraits = llvm::GraphTraits<PDGraph *>;

    for (const auto &e : llvm::make_range(PDGraphTraits::nodes_begin(&G),
                                          PDGraphTraits::nodes_end(&G))) {
      auto src = G.getOrInsertNode(e->get());

      for (const auto &k : llvm::make_range(PDGraphTraits::child_begin(e),
                                            PDGraphTraits::child_end(e))) {
        auto dst = G.getOrInsertNode(k->get());
        src->addDependentNode(dst, {});
      }
    }
  }
};

} // namespace pedigree end

#endif // header
