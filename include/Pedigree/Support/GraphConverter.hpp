//
//
//

#ifndef PEDIGREE_GRAPHCONVERTER_HPP
#define PEDIGREE_GRAPHCONVERTER_HPP

#include "Pedigree/Config.hpp"

#include "Pedigree/Analysis/Units/UnitTraits.hpp"

#include "llvm/ADT/GraphTraits.h"
// using llvm::GraphTraits

#include <memory>
// using std::addressof

namespace pedigree {

template <typename FromNodeT, typename ToNodeT,
          typename SourceConversionOperation, typename DestConversionOperation>
void Convert(const GenericDependenceGraph<FromNodeT> &From,
             GenericDependenceGraph<ToNodeT> &To,
             SourceConversionOperation SrcConvertOp,
             DestConversionOperation DstConvertOp) {
  using FromUnitT = typename FromNodeT::UnitType;
  using ToUnitT = typename ToNodeT::UnitType;

  static_assert(is_unit_convertible_v<FromUnitT, ToUnitT>,
                "Graph node undelying units are not convertible!");

  using GT = llvm::GraphTraits<typename std::add_pointer_t<
      typename std::remove_reference_t<decltype(From)>>>;

  for (const auto &node : GT::nodes(std::addressof(From))) {
    if (!node->unit()) {
      continue;
    }

    const auto &srcRange = SrcConvertOp(node->unit());

    for (const auto &child : GT::children(node)) {
      if (!child->unit()) {
        continue;
      }

      auto info = node->getEdgeInfo(child);
      const auto &dstRange = DstConvertOp(child->unit());

      for (auto &src : srcRange) {
        auto srcDep =
            To.getOrInsertNode(const_cast<ToUnitT>(std::addressof(src)));

        for (auto &dst : dstRange) {
          auto dstDep =
              To.getOrInsertNode(const_cast<ToUnitT>(std::addressof(dst)));
          srcDep->addDependentNode(dstDep, info.value());
        }
      }
    }
  }
}

} // namespace pedigree

#endif // header
