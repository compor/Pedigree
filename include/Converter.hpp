//
//
//

#ifndef PEDIGREE_CONVERTER_HPP
#define PEDIGREE_CONVERTER_HPP

#include "Config.hpp"

#include "traits/Unit.hpp"

template <typename FromNodeT, typename ToNodeT>
void Convert(const GenericDependenceGraph<FromNodeT> &From,
             GenericDependenceGraph<ToNodeT> &To) {
  using FromUnderT = typename FromNodeT::UnderlyingType;
  using ToUnderT = typename ToNodeT::UnderlyingType;

  static_assert(
      is_unit_adaptable<typename unit_traits<FromUnderT>::unit_category,
                        typename unit_traits<ToUnderT>::unit_category>::value,
      "Graph node undelying units are not adaptable!");
}

} // namespace pedigree end

#endif // header
