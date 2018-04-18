//
//
//

#ifndef PEDIGREE_DEPENDENCEINFO_INFO_DOTTRAITS_HPP
#define PEDIGREE_DEPENDENCEINFO_INFO_DOTTRAITS_HPP

#include "Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename InfoT> struct DependenceInfoDOTTraits {
  static std::string toDOTAttributes(const InfoT &I) {
    return I.toDOTAttributes();
  }
};

//

template <> struct DependenceInfoDOTTraits<void> {
  template <typename U> static std::string toDOTAttributes(const U &&) {
    return "";
  }
};

} // namespace pedigree

#endif // header
