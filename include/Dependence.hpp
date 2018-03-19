//
//
//

#ifndef DEPENDENCE_HPP
#define DEPENDENCE_HPP

#include <bitset>
// using std::bitset

#include <cstddef>
// using std::size_t

#include <type_traits>
// using std::underlying_type

namespace pedigree {

struct NoDependenceInfo {};

//

// also known as load-store classification (see OCMA book)
enum class DependenceHazard : std::size_t {
  // unknown = 0,
  flow,
  anti,
  out,
};

enum class DependenceOrigin : std::size_t {
  // unknown = 0,
  data,
  memory,
  control,
};

class BasicDependenceInfo {
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
};

// traits

template <typename InfoT> struct DependenceInfoTraits {
  static std::string toDOTAttributes(const InfoT &I) {
    return I.toDOTAttributes();
  }
};

template <> struct DependenceInfoTraits<NoDependenceInfo> {
  static std::string toDOTAttributes(const NoDependenceInfo &I) {
    return std::string{"color=blue;"};
  }
};

} // namespace pedigree end

#endif // header
