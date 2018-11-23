//
//
//

#ifndef PEDIGREE_ASSIGNNODEUID_HPP
#define PEDIGREE_ASSIGNNODEUID_HPP

#include "pedigree/Config.hpp"

#include "pedigree/Support/Utils/UniformObjectAccess.hpp"

#include <memory>
// using std::addressof

#include <iterator>
// using std::iterator_traits

#include <algorithm>
// using std::for_each

#include <type_traits>
// using std::is_pointer
// using std::is_same
// using std::remove_pointer

namespace pedigree {

namespace detail {

template <typename GraphT>
using NodeInfoType = typename GraphT::NodeType::NodeInfoType::value_type;

} // namespace detail

template <typename GraphT, typename IteratorT>
void AssignNodeUID(GraphT &G, IteratorT Begin, IteratorT End,
                   typename detail::NodeInfoType<GraphT>::IDTy FirstID = 0) {
  static_assert(!std::is_pointer<GraphT>::value,
                "Graph type cannot be a pointer!");

  using it_value_type = typename std::iterator_traits<IteratorT>::value_type;

  static_assert(
      std::is_same<std::remove_pointer_t<typename GraphT::NodeType::UnitType>,
                   std::remove_pointer_t<it_value_type>>::value,
      "Graph node unit does not match iteration unit!");

  std::for_each(Begin, End, [&](auto &e) {
    ++FirstID;

    auto &k = ToObj(e);
    auto node = G.getNode(std::addressof(k));

    if (node) {
      auto &info = (*node)->info();
      info.id = FirstID;
    }
  });
}

} // namespace pedigree

#endif // header
