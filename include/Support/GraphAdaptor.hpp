//
//
//

#ifndef PEDIGREE_GRAPHADAPTOR_HPP
#define PEDIGREE_GRAPHADAPTOR_HPP

#include "Config.hpp"

#include "Support/Traits/Unit.hpp"

#include "llvm/IR/BasicBlock.h"
// using llvm::BasicBlock

#include <memory>
// using std::addressof

#include <type_traits>
// using std::remove_reference
// using std::add_pointer

namespace pedigree {

template <typename FromNodeT, typename ToNodeT, typename AdaptOperation>
void Adapt(const GenericDependenceGraph<FromNodeT> &From,
           GenericDependenceGraph<ToNodeT> &To, AdaptOperation Adapt) {
  using FromUnderlyingT = typename FromNodeT::UnderlyingType;
  using ToUnderlyingT = typename ToNodeT::UnderlyingType;

  static_assert(is_unit_adaptable<
                    typename unit_traits<FromUnderlyingT>::unit_category,
                    typename unit_traits<ToUnderlyingT>::unit_category>::value,
                "Graph node undelying units are not adaptable!");

  using GT = llvm::GraphTraits<typename std::add_pointer_t<
      typename std::remove_reference_t<decltype(From)>>>;

  for (const auto &node : GT::nodes(std::addressof(From))) {
    auto src = To.getOrInsertNode(Adapt(node->get()));

    for (const auto &child : GT::children(node)) {
      auto adapted = Adapt(child->get());
      auto dst = To.getOrInsertNode(adapted);
      src->addDependentNode(dst, {});
    }
  }
}

} // namespace pedigree end

#endif // header
