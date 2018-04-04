//
//
//

#ifndef PEDIGREE_BASICDEPENDENCEINFO_HPP
#define PEDIGREE_BASICDEPENDENCEINFO_HPP

#include "Config.hpp"

#include "Traits.hpp"

#include "boost/operators.hpp"
// using boost::orable

#include "flags/flags.hpp"

#include <sstream>
// using std::stringstream

#include <vector>
// using std::vector

#include <algorithm>
// using std::copy

#include <iterator>
// using std::ostream_iterator

#include <cstddef>
// using std::size_t

namespace pedigree {

// also known as load-store classification (see OCMA book)
enum class DependenceHazard : std::size_t {
  // Unknown = 0,
  Flow,
  Anti,
  Out,
};

enum class DependenceOrigin : std::size_t {
  // Unknown = 0,
  Data,
  Memory,
  Control,
};

} // namespace pedigree end

ALLOW_FLAGS_FOR_ENUM(pedigree::DependenceHazard);
ALLOW_FLAGS_FOR_ENUM(pedigree::DependenceOrigin);

namespace pedigree {

struct BasicDependenceInfo : boost::orable<BasicDependenceInfo> {
  flags::flags<DependenceHazard> hazards;
  flags::flags<DependenceOrigin> origins;

  BasicDependenceInfo &operator|=(const BasicDependenceInfo &Other) {
    this->hazards = this->hazards | Other.hazards;
    this->origins = this->origins | Other.origins;
    return *this;
  }
};

// traits

template <> struct DependenceInfoTraits<BasicDependenceInfo> {
  static std::string toDOTAttributes(const BasicDependenceInfo &I) {
    auto attr = toDOTColor(I);

    if (I.origins & DependenceOrigin::Memory)
      attr += " " + toDOTLabel(I);

    return attr;
  }

  static std::string toDOTColor(const BasicDependenceInfo &I) {
    std::stringstream colorAttribute{};
    std::vector<std::string> colors{3};

    colorAttribute << "color=\"";

    if (I.origins.empty()) {
      colors.emplace_back("gray");
    } else {
      if (I.origins & DependenceOrigin::Control)
        colors.emplace_back("red;0.33");

      if (I.origins & DependenceOrigin::Memory)
        colors.emplace_back("purple;0.33");

      if (I.origins & DependenceOrigin::Data)
        colors.emplace_back("blue;0.33");
    }

    std::copy(colors.begin(), colors.end(),
              std::ostream_iterator<std::string>(colorAttribute, ":"));

    colorAttribute << "\"";

    return colorAttribute.str();
  }

  static std::string toDOTLabel(const BasicDependenceInfo &I) {
    std::string label{"label=\""};

    if (I.hazards.empty()) {
      label = "label=\"U\"";
    } else {
      if (I.hazards & DependenceHazard::Flow)
        label += "F";

      if (I.hazards & DependenceHazard::Anti)
        label += "A";

      if (I.hazards & DependenceHazard::Out)
        label += "O";

      label += "\"";
    }

    return label;
  }
};

} // namespace pedigree end

#endif // header
