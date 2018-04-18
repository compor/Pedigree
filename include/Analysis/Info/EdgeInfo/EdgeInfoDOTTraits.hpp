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
  static std::string toDOTAttributes(const InfoT &I) {
    return I.toDOTAttributes();
  }
};

//

template <> struct EdgeInfoDOTTraits<void> {
  template <typename U> static std::string toDOTAttributes(const U &&) {
    return "";
  }
};

} // namespace pedigree

#endif // header
