//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename T> struct EdgeInfoDOTTraits {
  static std::string toDOTAttributes(const T &) { return ""; }
};

} // namespace pedigree

