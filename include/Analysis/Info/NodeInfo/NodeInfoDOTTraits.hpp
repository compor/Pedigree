//
//
//

#ifndef PEDIGREE_INFO_NODEINFO_DOTTRAITS_HPP
#define PEDIGREE_INFO_NODEINFO_DOTTRAITS_HPP

#include "Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename InfoT> struct NodeInfoDOTTraits {
  static std::string toDOTAttributes(const InfoT &) { return ""; }
};

} // namespace pedigree

#endif // header
