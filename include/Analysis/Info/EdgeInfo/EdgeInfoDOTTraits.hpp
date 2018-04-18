//
//
//

#ifndef PEDIGREE_EDGEINFO_INFO_DOTTRAITS_HPP
#define PEDIGREE_EDGEINFO_INFO_DOTTRAITS_HPP

#include "Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename InfoT> struct EdgeInfoDOTTraits {
  static std::string toDOTAttributes(const InfoT &I) { return ""; }
};

} // namespace pedigree

#endif // header
