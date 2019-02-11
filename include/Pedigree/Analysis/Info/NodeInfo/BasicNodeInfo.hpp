//
//
//

#pragma once

#include "Pedigree/Config.hpp"

#include "NodeInfoDOTTraits.hpp"

#include <cstdint>
// using std::uint64_t

#include <sstream>
// using std::stringstream

#include <string>
// using std::string

namespace pedigree {

struct BasicNodeInfo {
  struct value_type {
    using IDTy = std::uint64_t;
    IDTy id;
    bool filtered;

    constexpr value_type() noexcept : id(0), filtered(false) {}
  };
};

// traits

template <> struct NodeInfoDOTTraits<BasicNodeInfo::value_type> {
  static std::string toDOTAttributes(const BasicNodeInfo::value_type &I) {
    std::stringstream ss{};
    ss << " dg_uid=" << I.id;

    return ss.str();
  }

  static bool isHidden(const BasicNodeInfo::value_type &I) {
    return I.filtered;
  }
};

} // namespace pedigree

