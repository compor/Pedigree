//
//
//

#ifndef PEDIGREE_DEPENDENCEINFO_TRAITS_HPP
#define PEDIGREE_DEPENDENCEINFO_TRAITS_HPP

#include "Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename InfoT> struct DependenceInfoTraits {
  static std::string toDOTAttributes(const InfoT &I) {
    return I.toDOTAttributes();
  }
};

//

template <> struct DependenceInfoTraits<void> {
  template <typename U> static std::string toDOTAttributes(const U &&) {
    return "";
  }
};

} // namespace pedigree

#endif // header
