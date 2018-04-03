//
//
//

#ifndef DEPENDENCE_HPP
#define DEPENDENCE_HPP

#include "boost/operators.hpp"
// using boost::orable

#include "flags/flags.hpp"

#include <bitset>
// using std::bitset

#include <string>
// using std::string

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

#include <type_traits>
// using std::underlying_type

namespace pedigree {

// also known as load-store classification (see OCMA book)
enum class DependenceHazard : std::size_t {
  // unknown = 0,
  Flow,
  Anti,
  Out,
};

enum class DependenceOrigin : std::size_t {
  // unknown = 0,
  Data,
  Memory,
  Control,
};

} // namespace pedigree end

ALLOW_FLAGS_FOR_ENUM(pedigree::DependenceHazard);
ALLOW_FLAGS_FOR_ENUM(pedigree::DependenceOrigin);

namespace pedigree {

struct NoDependenceInfo : private boost::orable<NoDependenceInfo> {
  NoDependenceInfo &operator|=(const NoDependenceInfo &) { return *this; }
};

//

// TODO maybe use this as an aggregate/result of orable operations
class BasicDependenceInfo : boost::orable<BasicDependenceInfo> {
  using DependenceHazardTy = std::underlying_type<DependenceHazard>::type;
  using DependenceOriginTy = std::underlying_type<DependenceOrigin>::type;

  std::bitset<sizeof(DependenceHazardTy)> hazard;
  std::bitset<sizeof(DependenceOriginTy)> origin;

public:
  void setHazard(DependenceHazard Hazard) {
    this->hazard.set(static_cast<DependenceHazardTy>(Hazard));
  }

  bool isHazard(DependenceHazard Hazard) const {
    return this->hazard.test(static_cast<DependenceHazardTy>(Hazard));
  }

  bool isUknownHazard() const { return this->hazard.none(); }

  void setOrigin(DependenceOrigin Origin) {
    this->origin.set(static_cast<DependenceOriginTy>(Origin));
  }

  bool isOrigin(DependenceOrigin Origin) const {
    return this->origin.test(static_cast<DependenceOriginTy>(Origin));
  }

  bool isUknownOrigin() const { return this->origin.none(); }

  BasicDependenceInfo &operator|=(const BasicDependenceInfo &Other) {
    this->hazard |= Other.hazard;
    this->origin |= Other.origin;
    return *this;
  }
};

// traits

template <typename InfoT> struct DependenceInfoTraits {
  static std::string toDOTAttributes(const InfoT &I) {
    return I.toDOTAttributes();
  }
};

template <> struct DependenceInfoTraits<NoDependenceInfo> {
  static std::string toDOTAttributes(const NoDependenceInfo &I) {
    return std::string{"color=black"};
  }
};

template <> struct DependenceInfoTraits<BasicDependenceInfo> {
  static std::string toDOTAttributes(const BasicDependenceInfo &I) {
    auto attr = toDOTColor(I);

    if (I.isOrigin(DependenceOrigin::Memory))
      attr += " " + toDOTLabel(I);

    return attr;
  }

  static std::string toDOTColor(const BasicDependenceInfo &I) {
    std::stringstream colorAttribute{};
    std::vector<std::string> colors{3};

    colorAttribute << "color=\"";

    if (I.isOrigin(DependenceOrigin::Control))
      colors.emplace_back("red;0.33");

    if (I.isOrigin(DependenceOrigin::Memory))
      colors.emplace_back("purple;0.33");

    if (I.isOrigin(DependenceOrigin::Data))
      colors.emplace_back("blue;0.33");

    if (I.isUknownOrigin()) {
      colors.clear();
      colors.emplace_back("gray");
    }

    std::copy(colors.begin(), colors.end(),
              std::ostream_iterator<std::string>(colorAttribute, ":"));

    colorAttribute << "\"";

    return colorAttribute.str();
  }

  static std::string toDOTLabel(const BasicDependenceInfo &I) {
    std::string label{"label=\""};

    if (I.isHazard(DependenceHazard::Flow))
      label += "F";

    if (I.isHazard(DependenceHazard::Anti))
      label += "A";

    if (I.isHazard(DependenceHazard::Out))
      label += "O";

    label += "\"";

    if (I.isUknownHazard())
      label = "label=\"U\"";

    return label;
  }
};

} // namespace pedigree end

#endif // header
