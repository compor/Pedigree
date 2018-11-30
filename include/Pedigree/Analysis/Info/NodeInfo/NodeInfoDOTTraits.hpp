//
//
//

#ifndef PEDIGREE_INFO_NODEINFO_DOTTRAITS_HPP
#define PEDIGREE_INFO_NODEINFO_DOTTRAITS_HPP

#include "Pedigree/Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename InfoT> struct NodeInfoDOTTraits {
  static std::string toDOTAttributes(const InfoT &) { return ""; }
  static bool isHidden(const InfoT &) { return false; }
};

} // namespace pedigree

#endif // header
