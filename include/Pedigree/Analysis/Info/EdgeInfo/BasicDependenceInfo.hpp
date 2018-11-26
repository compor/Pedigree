//
//
//

#ifndef PEDIGREE_INFO_BASICDEPENDENCEINFO_HPP
#define PEDIGREE_INFO_BASICDEPENDENCEINFO_HPP

#include "Pedigree/Config.hpp"

#include "EdgeInfoDOTTraits.hpp"

#include "boost/operators.hpp"
// using boost::orable

#include "flags/flags.hpp"
// using ALLOW_FLAGS_FOR_ENUM

#include <sstream>
// using std::stringstream

#include <string>
// using std::string

#include <iomanip>
// using std::setprecision

#include <vector>
// using std::vector

#include <algorithm>
// using std::copy

#include <iterator>
// using std::ostream_iterator

#include <cstddef>
// using std::size_t

namespace pedigree {

enum class DependenceHazard : std::size_t;

enum class DependenceOrigin : std::size_t;

} // namespace pedigree

ALLOW_FLAGS_FOR_ENUM(pedigree::DependenceHazard);
ALLOW_FLAGS_FOR_ENUM(pedigree::DependenceOrigin);

namespace pedigree {

// also known as load-store classification (see OCMA book)
enum class DependenceHazard : std::size_t {
  Unknown = 0,
  Flow = 0b01,
  Anti = 0b10,
  Out = 0b100,
};

enum class DependenceOrigin : std::size_t {
  Unknown = 0,
  Data = 0b01,
  Memory = 0b10,
  Control = 0b100,
};

struct BasicDependenceInfo {
  struct value_type : boost::orable<value_type> {
    flags::flags<DependenceOrigin> origins;
    flags::flags<DependenceHazard> hazards;

    constexpr value_type(DependenceOrigin origin,
                         DependenceHazard hazard) noexcept
        : origins(origin), hazards(hazard) {}

    constexpr value_type() noexcept
        : value_type(DependenceOrigin::Unknown, DependenceHazard::Unknown) {}

    constexpr value_type(const value_type &) = default;

    value_type &operator|=(const value_type &Other) noexcept {
      this->hazards = this->hazards | Other.hazards;
      this->origins = this->origins | Other.origins;

      return *this;
    }
  };
};

// traits

template <> struct EdgeInfoDOTTraits<BasicDependenceInfo::value_type> {
  static std::string toDOTAttributes(const BasicDependenceInfo::value_type &I) {
    auto attr = toDOTColor(I);

    if (I.origins & DependenceOrigin::Memory) {
      attr += " " + toDOTLabel(I);
    }

    return attr;
  }

  static std::string toDOTColor(const BasicDependenceInfo::value_type &I) {
    std::stringstream colorAttribute{};
    std::stringstream sep{};
    std::vector<std::string> colors{};

    colorAttribute << "color=\"";

    if (I.origins.empty()) {
      colors.emplace_back("grey");
    } else {
      auto n = std::distance(I.origins.cbegin(), I.origins.cend());
      auto ratio = 1.0 / n;
      sep << std::setprecision(2) << ";" << ratio << ":";

      if (I.origins & DependenceOrigin::Control) {
        colors.emplace_back("red");
      }

      if (I.origins & DependenceOrigin::Memory) {
        colors.emplace_back("purple");
      }

      if (I.origins & DependenceOrigin::Data) {
        colors.emplace_back("blue");
      }
    }

    std::copy(
        colors.begin(), colors.end(),
        std::ostream_iterator<std::string>(colorAttribute, sep.str().c_str()));

    colorAttribute << "\"";

    return colorAttribute.str();
  }

  static std::string toDOTLabel(const BasicDependenceInfo::value_type &I) {
    std::string label{"label=\""};

    if (I.hazards.empty()) {
      label = "label=\"U\"";
    } else {
      if (I.hazards & DependenceHazard::Flow) {
        label += "F";
      }

      if (I.hazards & DependenceHazard::Anti) {
        label += "A";
      }

      if (I.hazards & DependenceHazard::Out) {
        label += "O";
      }

      label += "\"";
    }

    return label;
  }
};

} // namespace pedigree

#endif // header
