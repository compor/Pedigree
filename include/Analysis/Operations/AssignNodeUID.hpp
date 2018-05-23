//
//
//

#ifndef PEDIGREE_ASSIGNNODEUID_HPP
#define PEDIGREE_ASSIGNNODEUID_HPP

#include "Config.hpp"

#include <iterator>
// using std::iterator_traits

#include <algorithm>
// using std::for_each

#include <type_traits>
// using std::is_pointer
// using std::is_same

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
  static_assert(
      std::is_same<typename GraphT::NodeType::UnitType,
                   typename std::iterator_traits<IteratorT>::value_type>::value,
      "Graph node unit does not match iteration unit!");

  std::for_each(Begin, End, [&](const auto &e) {
    ++FirstID;
    auto node = G.getNode(e);

    if (node) {
      auto &info = (*node)->info();
      info.id = FirstID;
    }
  });
}

} // namespace pedigree

#endif // header
