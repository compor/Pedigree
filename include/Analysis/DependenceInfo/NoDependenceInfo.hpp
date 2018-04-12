//
//
//

#ifndef PEDIGREE_NODEPENDENCEINFO_HPP
#define PEDIGREE_NODEPENDENCEINFO_HPP

#include "Config.hpp"

#include "Traits.hpp"

#include "boost/operators.hpp"
// using boost::orable

#include <string>
// using std::string

namespace pedigree {

struct NoDependenceInfo : private boost::orable<NoDependenceInfo> {
  NoDependenceInfo &operator|=(const NoDependenceInfo &) noexcept {
    return *this;
  }
};

// traits

template <> struct DependenceInfoTraits<NoDependenceInfo> {
  static std::string toDOTAttributes(const NoDependenceInfo &I) {
    return std::string{"color=black"};
  }
};

} // namespace pedigree

#endif // header include
