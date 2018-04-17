//
//
//

#ifndef PEDIGREE_GRAPHCONVERTER_HPP
#define PEDIGREE_GRAPHCONVERTER_HPP

#include "Config.hpp"

#include "Analysis/Units/UnitTraits.hpp"

#include <memory>
// using std::addressof

namespace pedigree {

template <typename FromNodeT, typename ToNodeT, typename ConversionOperation>
void Convert(const GenericDependenceGraph<FromNodeT> &From,
             GenericDependenceGraph<ToNodeT> &To,
             ConversionOperation ConvertOp) {
  using FromUnderlyingT = typename FromNodeT::UnderlyingType;
  using ToUnderlyingT = typename ToNodeT::UnderlyingType;

  static_assert(is_unit_convertible_v<FromUnderlyingT, ToUnderlyingT>,
                "Graph node undelying units are not convertible!");

  using GT = llvm::GraphTraits<typename std::add_pointer_t<
      typename std::remove_reference_t<decltype(From)>>>;

  for (const auto &node : GT::nodes(std::addressof(From))) {
    auto src = To.getOrInsertNode(ConvertOp(node->get()));

    for (const auto &child : GT::children(node)) {
      auto converted = ConvertOp(child->get());
      auto dst = To.getOrInsertNode(converted);
      auto info = node->getEdgeInfo(child);
      src->addDependentNode(dst, info.value());
    }
  }
}

} // namespace pedigree

#endif // header
