//
//
//

#ifndef PEDIGREE_INFO_GENERALINFO_HPP
#define PEDIGREE_INFO_GENERALINFO_HPP

#include "Pedigree/Config.hpp"

#include "NodeInfoDOTTraits.hpp"

#include <cstdint>
// using std::uint64_t

#include <sstream>
// using std::stringstream

#include <string>
// using std::string

namespace pedigree {

struct GeneralInfo {
  struct value_type {
    using IDTy = std::uint64_t;
    IDTy id;
    bool filtered;

    constexpr value_type() noexcept : id(0), filtered(false) {}
  };
};

// traits

template <> struct NodeInfoDOTTraits<GeneralInfo::value_type> {
  static std::string toDOTAttributes(const GeneralInfo::value_type &I) {
    std::stringstream ss{};
    ss << " dg_uid=" << I.id;

    return ss.str();
  }

  static bool isHidden(const GeneralInfo::value_type &I) { return I.filtered; }
};

} // namespace pedigree

#endif // header
