//
//
//

#ifndef PEDIGREE_INFO_UIDINFO_HPP
#define PEDIGREE_INFO_UIDINFO_HPP

#include "Config.hpp"

#include "NodeInfoDOTTraits.hpp"

#include <cstdint>
// using std::uint64_t

#include <sstream>
// using std::stringstream

#include <string>
// using std::string

namespace pedigree {

struct UIDInfo {
  struct value_type {
    using IDTy = std::uint64_t;
    IDTy id;

    constexpr value_type() noexcept : id(0) {}
  };
};

// traits

template <> struct NodeInfoDOTTraits<UIDInfo::value_type> {
  static std::string toDOTAttributes(const UIDInfo::value_type &I) {
    std::stringstream ss{};
    ss << " dg_uid=" << I.id;

    return ss.str();
  }
};

} // namespace pedigree

#endif // header
