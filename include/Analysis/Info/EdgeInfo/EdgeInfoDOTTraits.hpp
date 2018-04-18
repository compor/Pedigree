//
//
//

#ifndef PEDIGREE_INFO_EDGEINFO_DOTTRAITS_HPP
#define PEDIGREE_INFO_EDGEINFO_DOTTRAITS_HPP

#include "Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename T> struct EdgeInfoDOTTraits {
  static std::string toDOTAttributes(const T &) { return ""; }
};

} // namespace pedigree

#endif // header
