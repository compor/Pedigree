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

enum class DependenceType : std::size_t {
  unknown = 0,
  flow,
  anti,
  out,
};

enum class DependenceOrigin : std::size_t {
  unknown = 0,
  data,
  memory,
  control,
};

class BasicDependenceInfo {
  using DependenceTypeTy = std::underlying_type<DependenceType>::type;
  using DependenceOriginTy = std::underlying_type<DependenceOrigin>::type;

  std::bitset<sizeof(DependenceTypeTy)> type;
  std::bitset<sizeof(DependenceOriginTy)> origin;

public:
  void setType(DependenceType DepType) {
    this->type.set(static_cast<DependenceTypeTy>(DepType));
  }

  bool isType(DependenceType DepType) {
    return this->type.test(static_cast<DependenceTypeTy>(DepType));
  }

  void setOrigin(DependenceOrigin Origin) {
    this->type.set(static_cast<DependenceOriginTy>(Origin));
  }

  bool isOrigin(DependenceOrigin Origin) {
    return this->origin.test(static_cast<DependenceOriginTy>(Origin));
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
    return std::string{"color=blue;"};
  }
};

} // namespace pedigree end

#endif // header
