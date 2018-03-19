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

template <typename InfoT> struct DependenceInfoTraits {
  static std::string toDOTAttributes(const InfoT &I) {
    return I.toDOTAttributes();
  }
};

struct NoDependenceInfo {};

template <> struct DependenceInfoTraits<NoDependenceInfo> {
  static std::string toDOTAttributes(const NoDependenceInfo &I) {
    return std::string{"color=purple"};
  }
};

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

struct BasicDependenceInfo {
  std::bitset<sizeof(std::underlying_type<DependenceType>::type)> type;
  std::bitset<sizeof(std::underlying_type<DependenceOrigin>::type)> origin;
};

} // namespace pedigree end

#endif // header
