//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include <string>
// using std::string

namespace pedigree {

template <typename InfoT> struct NodeInfoDOTTraits {
  static std::string toDOTAttributes(const InfoT &) { return ""; }
  static bool isHidden(const InfoT &) { return false; }
};

} // namespace pedigree

